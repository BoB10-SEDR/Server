#include "CEpollServer.h"
#include "CEpollServerException.h"
#include "CMessage.h"

CEpollServer::CEpollServer()
{
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(atoi("3000"));

	CEpollServer::CreateEpoll();
}

CEpollServer::CEpollServer(std::string port)
{
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(atoi(port.c_str()));
	
	CEpollServer::CreateEpoll();
}

CEpollServer::CEpollServer(std::string ip, std::string port)
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
	LoggerManager()->Info("Start...........\n");
	serverSocket = socket(PF_INET, SOCK_STREAM, 0);

	if (serverSocket < 0)
	{
		throw CEpollServerException("Socket Create Fail");
	}

	while (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) 
	{
		sleep(5);
		LoggerManager()->Error("Bind...........\n");
	}

	while (listen(serverSocket, requestCount) < 0)
	{
		sleep(5);
		LoggerManager()->Error("Listen...........\n");
	}

	CEpollServer::PushEpoll("", serverSocket, EPOLLIN);

	return 0;
}

int CEpollServer::Send(std::string agentInfo, std::string message)
{
	int agentSocket = SearchAgent(agentInfo);

	if (agentSocket <= 0)
	{
		LoggerManager()->Warn("Agent not found...........\n");
		return -1;
	}

	write(agentSocket, message.c_str(), message.length());
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
			throw CEpollServerException("epoll_wait() error");

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
					throw CEpollServerException("Agent Socket fcntl() error");

				if (agentSocket < 0)
				{
					LoggerManager()->Warn(StringFormatter("Agent Accept Error [%d]...........\n", agentSocket));
					close(agentSocket);
					continue;
				}

				PushEpoll(ConvertAgentInfo(agentAddress), agentSocket, EPOLLIN | EPOLLET);
				LoggerManager()->Info(StringFormatter("Agent Connect [%s] [%d]...........\n", SearchAgent(agentSocket).c_str(), agentSocket));
			}
			else
			{
				int agentSocket = epollEvents[i].data.fd;
				int messageLength;
				char message[BUFFER_SIZE];

				messageLength = read(agentSocket, &message, BUFFER_SIZE);

				if (messageLength <= 0)
				{
					LoggerManager()->Info(StringFormatter("Agent Disconnect [%s] [%d]...........\n", SearchAgent(agentSocket).c_str(), agentSocket));
					PopEpoll(agentSocket);
				}
				else
				{
					message[messageLength] = 0;

					ST_PACKET_INFO* stPacketRead = new ST_PACKET_INFO();
					core::ReadJsonFromString(stPacketRead, message);
					MessageManager()->PushReceiveMessage(SearchAgent(agentSocket), stPacketRead);
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

	LoggerManager()->Info("Terminate...........\n");
	return 0;
}

int CEpollServer::CreateEpoll()
{
	epollFD = epoll_create(EPOLL_SIZE);
	if (epollFD < 0)
		throw CEpollServerException("CreateEpoll Fail");

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
		throw CEpollServerException("PushEpoll Fail");

	if (agentSocket != serverSocket && agentSocket != 0)
	{
		agentInfoKeyLists.insert(std::pair<std::string, int>(agentInfo, agentSocket));
		agentSocketKeyLists.insert(std::pair<int, std::string>(agentSocket, agentInfo));
	}

	return 0;
}

int CEpollServer::PopEpoll(int agentSocket)
{
	std::string agentInfo = SearchAgent(agentSocket);
	
	if (epoll_ctl(epollFD, EPOLL_CTL_DEL, agentSocket, NULL) < 0)
		throw CEpollServerException("PopEpoll Fail");

	close(agentSocket);

	agentInfoKeyLists.erase(agentInfo);
	agentSocketKeyLists.erase(agentSocket);
	return 0;
}

int CEpollServer::SearchAgent(std::string agentInfo)
{
	int agentSocket = agentInfoKeyLists[agentInfo];
	return agentSocket;
}

std::string CEpollServer::SearchAgent(int agentSocket)
{
	std::string agentInfo = agentSocketKeyLists[agentSocket];
	return agentInfo;
}

CEpollServer* CEpollServer::GetInstance()
{
	static CEpollServer instance;
	return &instance;
}

CEpollServer* CEpollServer::GetInstance(std::string port)
{
	static CEpollServer instance(port);
	return &instance;
}

CEpollServer* CEpollServer::GetInstance(std::string ip, std::string port)
{
	static CEpollServer instance(ip, port);
	return &instance;
}

std::string CEpollServer::ConvertAgentInfo(struct sockaddr_in agentAddress) {
	return inet_ntoa(agentAddress.sin_addr) + std::string(":") + std::to_string(ntohs(agentAddress.sin_port));
}