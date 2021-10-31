#include "CEpollServer.h"
#include "CMessage.h"

CEpollServer::CEpollServer()
{
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(atoi("3000"));

	CEpollServer::CreateEpoll();
}

CEpollServer::CEpollServer(std::tstring port)
{
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(atoi(port.c_str()));
	
	CEpollServer::CreateEpoll();
}

CEpollServer::CEpollServer(std::tstring ip, std::tstring port)
{
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(ip.c_str());
	serverAddress.sin_port = htons(atoi(port.c_str()));

	CEpollServer::CreateEpoll();
}

CEpollServer::~CEpollServer()
{
	CEpollServer::End();
}

int CEpollServer::Start(int requestCount)
{
	core::Log_Debug(TEXT("CEpollServer.cpp : %s"), TEXT("Working EpollServerStart In Thread"));
	serverSocket = socket(PF_INET, SOCK_STREAM, 0);

	if (serverSocket < 0)
		core::Log_Warn(TEXT("CEpollServer.cpp - [%s] : %d"), TEXT("Socket Create Fail"), errno);

	while (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) 
	{
		sleep(5);
		core::Log_Warn(TEXT("CEpollServer.cpp - [%s] : %d"), TEXT("Socket Bind Fail"), errno);
	}

	while (listen(serverSocket, requestCount) < 0)
	{
		sleep(5);
		core::Log_Warn(TEXT("CEpollServer.cpp - [%s] : %d"), TEXT("Socket Listen Fail"), errno);
	}

	CEpollServer::PushEpoll("", serverSocket, EPOLLIN);

	return 0;
}

int CEpollServer::Send(std::tstring agentInfo, std::tstring message)
{
	int agentSocket = SearchAgent(agentInfo);

	if (agentSocket <= 0)
	{
		core::Log_Warn(TEXT("CEpollServer.cpp - [%s] : %d"), TEXT("Agent Not Found"), TEXT(agentInfo.c_str()));
		return -1;
	}

	int result = write(agentSocket, message.c_str(), message.length());

	if (result == -1)
		core::Log_Warn(TEXT("CEpollServer.cpp - [%s] : %d"), TEXT("Send Error Code"), errno);
	else
		core::Log_Debug(TEXT("CEpollServer.cpp - [%s] : %d"), TEXT("Send Complete"), result);

	return 0;
}

int CEpollServer::Recv()
{
	int timeout = -1;
	while(1)
	{
		sleep(0);
		int eventCount = epoll_wait(epollFD, epollEvents, EPOLL_SIZE, timeout);

		if (eventCount < 0)
			core::Log_Warn(TEXT("CEpollServer.cpp - [%s] : %d"), TEXT("Epoll Wait Error"), errno);

		for (int i = 0; i < eventCount; i++)
		{
			if (epollEvents[i].data.fd == serverSocket)
			{
				int            agentSocket;
				int            agentLength;
				struct sockaddr_in    agentAddress;

				agentLength = sizeof(agentAddress);
				agentSocket = accept(serverSocket, (struct sockaddr*)&agentAddress, (socklen_t*)&agentLength);

				int flags = fcntl(agentSocket, F_GETFL);
				flags |= O_NONBLOCK;

				if (fcntl(agentSocket, F_SETFL, flags) < 0)
					core::Log_Warn(TEXT("CEpollServer.cpp - [%s] : %d"), TEXT("Agent Socket fcntl error"), errno);

				if (agentSocket < 0)
				{
					core::Log_Warn(TEXT("CEpollServer.cpp - [%s] : %d"), TEXT("Agent Socket Accept error"), errno);
					close(agentSocket);
					continue;
				}

				PushEpoll(ConvertAgentInfo(agentAddress), agentSocket, EPOLLIN | EPOLLET);
				core::Log_Info(TEXT("CEpollServer.cpp - [%s] : %s - %d"), TEXT("Agent Connect"), SearchAgent(agentSocket).c_str(), agentSocket);
			}
			else
			{
				int agentSocket = epollEvents[i].data.fd;
				int messageLength;
				char message[BUFFER_SIZE+1];
				while (1) {
					messageLength = read(agentSocket, &message, BUFFER_SIZE);
					core::Log_Debug(TEXT("CEpollServer.cpp - [%s] : %d"), TEXT("Read Complete"), messageLength);
					core::Log_Debug(TEXT("CEpollServer.cpp - [%s] : %s"), TEXT("Recieve Message"), message);

					if (messageLength == 0)
					{
						core::Log_Warn(TEXT("CEpollServer.cpp - [%s] : %s"), TEXT("Agent Disconneted"), TEXT(SearchAgent(agentSocket).c_str()));
						PopEpoll(agentSocket);
						break;
					}
					else if (messageLength < 0)
					{
						core::Log_Warn(TEXT("CEpollServer.cpp - [%s] : %d"), TEXT("Read Error Code"), errno);
						core::Log_Debug(TEXT("CEpollServer.cpp - [%s] : %s"), TEXT("Remain MessageBuffer"), TEXT(agentMessageBuffers[agentSocket].c_str()));
						break;
					}
					else
					{
						message[messageLength] = 0;
						agentMessageBuffers[agentSocket] += message;

						while (1)
						{
							size_t location = agentMessageBuffers[agentSocket].find("END");
							core::Log_Debug(TEXT("CEpollServer.cpp - [%s] : %d"), TEXT("Find End Position"), location);

							if (location == -1)
								break;

							ST_PACKET_INFO* stPacketRead = new ST_PACKET_INFO();
							core::ReadJsonFromString(stPacketRead, agentMessageBuffers[agentSocket].substr(0, location));

							MessageManager()->PushReceiveMessage(SearchAgent(agentSocket), stPacketRead);
							agentMessageBuffers[agentSocket] = agentMessageBuffers[agentSocket].substr(location + 3);
						}
					}
					message[0] = 0;
				}
			}
		}
	}
}

int CEpollServer::End()
{
	agentInfoKeyLists.clear();
	agentSocketKeyLists.clear();
	close(epollFD);
	close(serverSocket);
	serverSocket = -1;

	core::Log_Warn(TEXT("CEpollServer.cpp - [%s]"), TEXT("Server Terminate"));
	return 0;
}

int CEpollServer::CreateEpoll()
{
	epollFD = epoll_create(EPOLL_SIZE);
	if (epollFD < 0)
		core::Log_Warn(TEXT("CEpollServer.cpp - [%s] : [%d]"), TEXT("CreateEpoll Fail"), errno);

	epollEvents = (struct epoll_event*)malloc(sizeof(struct epoll_event) * EPOLL_SIZE);
	memset(epollEvents, 0, sizeof(struct epoll_event) * EPOLL_SIZE);
	return 0;
}

int CEpollServer::PushEpoll(std::string agentInfo, int agentSocket, int event)
{
	struct epoll_event epollEvent;
	epollEvent.events = event;
	epollEvent.data.fd = agentSocket;
	
	if (epoll_ctl(epollFD, EPOLL_CTL_ADD, agentSocket, &epollEvent) < 0)
		core::Log_Warn(TEXT("CEpollServer.cpp - [%s] : [%d]"), TEXT("PushEpoll Fail"), errno);

	if (agentSocket != serverSocket && agentSocket != 0)
	{
		agentInfoKeyLists.insert(std::pair<std::string, int>(agentInfo, agentSocket));
		agentSocketKeyLists.insert(std::pair<int, std::string>(agentSocket, agentInfo));
		agentMessageBuffers.insert(std::pair<int, std::string>(agentSocket, ""));
	}

	return 0;
}

int CEpollServer::PopEpoll(int agentSocket)
{
	std::string agentInfo = SearchAgent(agentSocket);
	
	if (epoll_ctl(epollFD, EPOLL_CTL_DEL, agentSocket, NULL) < 0)
		core::Log_Warn(TEXT("CEpollServer.cpp - [%s] : [%d]"), TEXT("PopEpoll Fail"), errno);

	close(agentSocket);

	agentInfoKeyLists.erase(agentInfo);
	agentSocketKeyLists.erase(agentSocket);
	return 0;
}

int CEpollServer::SearchAgent(std::tstring agentInfo)
{
	int agentSocket = agentInfoKeyLists.count(agentInfo) ? agentInfoKeyLists[agentInfo] : -1;
	return agentSocket;
}

std::string CEpollServer::SearchAgent(int agentSocket)
{
	std::string agentInfo = agentSocketKeyLists.count(agentSocket) ? agentSocketKeyLists[agentSocket] : "";
	return agentInfo;
}

CEpollServer* CEpollServer::GetInstance()
{
	static CEpollServer instance;
	return &instance;
}

CEpollServer* CEpollServer::GetInstance(std::tstring port)
{
	static CEpollServer instance(port);
	return &instance;
}

CEpollServer* CEpollServer::GetInstance(std::tstring ip, std::tstring port)
{
	static CEpollServer instance(ip, port);
	return &instance;
}

std::tstring CEpollServer::ConvertAgentInfo(struct sockaddr_in agentAddress) {
	return inet_ntoa(agentAddress.sin_addr) + std::tstring(":") + std::to_string(ntohs(agentAddress.sin_port));
};