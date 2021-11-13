#include "CAgentServer.h"
#include "CMessage.h"

extern ST_ENV env;

CAgentServer::CAgentServer()
{
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(atoi("3000"));

	CAgentServer::CreateEpoll();
}

CAgentServer::CAgentServer(std::tstring port)
{
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(atoi(port.c_str()));

	CAgentServer::CreateEpoll();
}

CAgentServer::CAgentServer(std::tstring ip, std::tstring port)
{
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(ip.c_str());
	serverAddress.sin_port = htons(atoi(port.c_str()));

	CAgentServer::CreateEpoll();
}

CAgentServer::~CAgentServer()
{
	CAgentServer::End();
}

void CAgentServer::Init()
{
	core::Log_Debug(TEXT("CAgentServer.cpp - [%s]"), TEXT("Init"));

	apiList.push_back(new CAgentApi());
}

void CAgentServer::Start()
{
	core::Log_Debug(TEXT("CAgentServer.cpp - [%s]"), TEXT("Working EpollServerStart In Thread"));
	int requestCount = 5;
	serverSocket = socket(PF_INET, SOCK_STREAM, 0);

	if (serverSocket < 0)
		core::Log_Warn(TEXT("CAgentServer.cpp - [%s] : %d"), TEXT("Socket Create Fail"), errno);

	while (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
	{
		sleep(5);
		core::Log_Warn(TEXT("CAgentServer.cpp - [%s] : %d"), TEXT("Socket Bind Fail"), errno);
	}

	while (listen(serverSocket, requestCount) < 0)
	{
		sleep(5);
		core::Log_Warn(TEXT("CAgentServer.cpp - [%s] : %d"), TEXT("Socket Listen Fail"), errno);
	}

	CAgentServer::PushEpoll(serverSocket, EPOLLIN);

	SetupRoutes();

	std::future<void> send = std::async(std::launch::async, &CAgentServer::Send, AgentServerManager());
	std::future<void> receive = std::async(std::launch::async, &CAgentServer::Recv, AgentServerManager());
	std::future<void> match = std::async(std::launch::async, &CAgentServer::MatchMessage, AgentServerManager());
}

void CAgentServer::SetupRoutes()
{
	for (auto i : apiList)
	{
		i->Routing(router);
	}
}

void CAgentServer::Send()
{
	core::Log_Debug(TEXT("CAgentServer.cpp - [%s]"), TEXT("Working SendMessage In Thread"));
	ST_MESSAGE_INFO* stMessageInfo;

	while (1) {
		sleep(0);
		stMessageInfo = MessageManager()->PopSendMessage();

		if (stMessageInfo == NULL)
			continue;
		
		if (SearchAgent(stMessageInfo->agentSocket) == -1) {
			core::Log_Warn(TEXT("CAgentServer.cpp - [%s] : %s"), TEXT("Agent Not Found"), stMessageInfo->agentSocket);
			continue;
		}

		ST_NEW_PACKET_INFO* stPacketInfo = new ST_NEW_PACKET_INFO(stMessageInfo->opcode, stMessageInfo->metaInfo);
		std::tstring jsPacketInfo;
		core::WriteJsonToString(stPacketInfo, jsPacketInfo);
		
		std::tstring message = TEXT("BOBSTART") + jsPacketInfo + TEXT("BOBEND");
		int result = write(stMessageInfo->agentSocket, message.c_str(), message.length());

		if (result == -1)
			core::Log_Warn(TEXT("CAgentServer.cpp - [%s] : %d"), TEXT("Send Error Code"), errno);
		else 
		{
			core::Log_Debug(TEXT("CAgentServer.cpp - [%s] : %d"), TEXT("Send Complete"), result);
			core::Log_Debug(TEXT("CMessage.cpp - [%s] : %d -> %s"), TEXT("Send Message"), stMessageInfo->agentSocket, TEXT(stMessageInfo->metaInfo.c_str()));

		}
		delete stMessageInfo;
		delete stPacketInfo;
	}
}

void CAgentServer::Recv()
{
	core::Log_Debug(TEXT("CAgentServer.cpp - [%s]"), TEXT("Working RecvMessage In Thread"));
	int timeout = -1;
	while (1)
	{
		sleep(0);
		int eventCount = epoll_wait(epollFD, epollEvents, EPOLL_SIZE, timeout);

		if (eventCount < 0)
			core::Log_Warn(TEXT("CAgentServer.cpp - [%s] : %d"), TEXT("Epoll Wait Error"), errno);

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
					core::Log_Warn(TEXT("CAgentServer.cpp - [%s] : %d"), TEXT("Agent Socket fcntl error"), errno);

				if (agentSocket < 0)
				{
					core::Log_Warn(TEXT("CAgentServer.cpp - [%s] : %d"), TEXT("Agent Socket Accept error"), errno);
					close(agentSocket);
					continue;
				}

				PushEpoll(agentSocket, EPOLLIN | EPOLLET);
				core::Log_Info(TEXT("CAgentServer.cpp - [%s] : %d"), TEXT("Agent Connect"), agentSocket);
			}
			else
			{
				int agentSocket = epollEvents[i].data.fd;
				int messageLength;
				char message[BUFFER_SIZE + 1];
				while (1) {
					messageLength = read(agentSocket, &message, BUFFER_SIZE);
					core::Log_Debug(TEXT("CAgentServer.cpp - [%s] : %d"), TEXT("Read Complete"), messageLength);
					core::Log_Debug(TEXT("CAgentServer.cpp - [%s] : %s"), TEXT("Recieve Message"), message);

					if (messageLength == 0)
					{
						core::Log_Warn(TEXT("CAgentServer.cpp - [%s] : %d"), TEXT("Agent Disconneted"), agentSocket);
						PopEpoll(agentSocket);
						break;
					}
					else if (messageLength < 0)
					{
						core::Log_Warn(TEXT("CAgentServer.cpp - [%s] : %d"), TEXT("Read Error Code"), errno);
						core::Log_Debug(TEXT("CAgentServer.cpp - [%s] : %s"), TEXT("Remain MessageBuffer"), TEXT(agentMessageBuffers[agentSocket].c_str()));
						break;
					}
					else
					{
						message[messageLength] = 0;
						agentMessageBuffers[agentSocket] += message;

						while (1)
						{
							size_t start_location = agentMessageBuffers[agentSocket].find("BOBSTART");
							size_t end_location = agentMessageBuffers[agentSocket].find("BOBEND");
							core::Log_Debug(TEXT("CAgentServer.cpp - [%s] : %d, %d"), TEXT("Find Position(Start, End)"), start_location, end_location);

							if (start_location == -1 || end_location == -1)
								break;

							ST_NEW_PACKET_INFO* stPacketInfo = new ST_NEW_PACKET_INFO();
							core::ReadJsonFromString(stPacketInfo, agentMessageBuffers[agentSocket].substr(start_location + 8, end_location - (start_location + 8)));

							MessageManager()->PushReceiveMessage(agentSocket, (OPCODE)stPacketInfo->opcode, stPacketInfo->data);
							delete stPacketInfo;

							agentMessageBuffers[agentSocket] = agentMessageBuffers[agentSocket].substr(end_location + 6);
						}
					}
					message[0] = 0;
				}
			}
		}
	}
}

void CAgentServer::MatchMessage()
{
	core::Log_Debug(TEXT("CAgentServer.cpp - [%s]"), TEXT("Working MatchReceiveMessage In Thread"));
	
	ST_MESSAGE_INFO* stMessageInfo;
	CAgentApi* agentApi = new CAgentApi();

	while (1)
	{
		sleep(0);
		stMessageInfo = MessageManager()->PopReceiveMessage();

		if (stMessageInfo == NULL)
			continue;
	
		core::Log_Debug(TEXT("CAgentServer.cpp - [%s] %d"), TEXT("Receive Packet"), TEXT(stMessageInfo->opcode));
		// 받은 메세지를 free 해주기 위해서, 그냥 Cagent api 에서 받은 메세지 주소를 프리 처리하는게 좋을꺼 같다.
		std::future<void> result = std::async(std::launch::async, router[(OPCODE)stMessageInfo->opcode], agentApi, stMessageInfo->agentSocket, stMessageInfo->metaInfo);
	}

	free(agentApi);
}

void CAgentServer::End()
{
	for (auto i : apiList)
		free(i);

	close(epollFD);
	close(serverSocket);
	serverSocket = -1;

	core::Log_Warn(TEXT("CAgentServer.cpp - [%s]"), TEXT("Server Terminate"));
}

int CAgentServer::CreateEpoll()
{
	epollFD = epoll_create(EPOLL_SIZE);
	if (epollFD < 0)
		core::Log_Warn(TEXT("CAgentServer.cpp - [%s] : [%d]"), TEXT("CreateEpoll Fail"), errno);

	epollEvents = (struct epoll_event*)malloc(sizeof(struct epoll_event) * EPOLL_SIZE);
	memset(epollEvents, 0, sizeof(struct epoll_event) * EPOLL_SIZE);
	return 0;
}

int CAgentServer::PushEpoll(int agentSocket, int event)
{
	struct epoll_event epollEvent;
	epollEvent.events = event;
	epollEvent.data.fd = agentSocket;

	if (epoll_ctl(epollFD, EPOLL_CTL_ADD, agentSocket, &epollEvent) < 0)
		core::Log_Warn(TEXT("CAgentServer.cpp - [%s] : [%d]"), TEXT("PushEpoll Fail"), errno);

	if (agentSocket != serverSocket && agentSocket != 0)
	{
		agentMessageBuffers.insert(std::pair<int, std::string>(agentSocket, ""));
	}
	return 0;
}

int CAgentServer::PopEpoll(int agentSocket)
{
	int agentFd = SearchAgent(agentSocket);

	if (epoll_ctl(epollFD, EPOLL_CTL_DEL, agentSocket, NULL) < 0)
		core::Log_Warn(TEXT("CAgentServer.cpp - [%s] : [%d]"), TEXT("PopEpoll Fail"), errno);

	MessageManager()->PushReceiveMessage(agentFd, DEVICE_DEAD, "");
	close(agentSocket);

	agentMessageBuffers.erase(agentSocket);
	return 0;
}

int CAgentServer::SearchAgent(int agentSocket)
{
	int agentFd = agentMessageBuffers.count(agentSocket) ? agentSocket : -1;

	return agentFd;
}

CAgentServer* CAgentServer::GetInstance()
{
	static CAgentServer instance(env.socketPort);
	return &instance;
}