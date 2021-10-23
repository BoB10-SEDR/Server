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
	serverSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0)
		throw CEpollServerException("socket create fail");

	if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
		throw CEpollServerException("bind fail");

	if (listen(serverSocket, requestCount) < 0)
		throw CEpollServerException("bind fail");

	CEpollServer::PushEpoll(serverSocket, EPOLLIN);

	printf("server Start\n");

	return 0;
}

int CEpollServer::Send(int deviceID, std::string message)
{
	int clientSocket = SearchClient(deviceID);
	if (clientSocket != -1) {
		write(clientSocket, message.c_str(), message.length());
		return 0;
	}
	return -1;
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
				int            clientSocket;
				int            clientLength;
				struct sockaddr_in    clientAddress;

				clientLength = sizeof(clientAddress);
				clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, (socklen_t*)&clientLength);

				int flags = fcntl(clientSocket, F_GETFL);
				flags |= O_NONBLOCK;

				if (fcntl(clientSocket, F_SETFL, flags) < 0)
					throw CEpollServerException("clientSocket fcntl() error");

				if (clientSocket < 0)
				{
					printf("accept() error [%d]\n", clientSocket);
					continue;
				}

				PushEpoll(clientSocket, EPOLLIN | EPOLLET);
				printf("Client Connect [%d]\n", clientSocket);
			}
			else
			{
				int clientSocket = epollEvents[i].data.fd;
				int messageLength;
				char message[BUFFER_SIZE];

				messageLength = read(clientSocket, &message, BUFFER_SIZE);

				if (messageLength <= 0)
				{
					printf("Client Disconnect [%d]\n", clientSocket);
					PopEpoll(clientSocket);
				}
				else
				{
					message[messageLength] = 0;

					ST_PACKET_INFO* stPacketRead = new ST_PACKET_INFO();
					core::ReadJsonFromString(stPacketRead, message);
					MessageManager()->PushReceiveMessage(clientSocket, stPacketRead);
				}
			}
		}
	}
}

int CEpollServer::End()
{
	clientLists.clear();
	close(epollFD);
	close(serverSocket);
	delete epollEvents;
	serverSocket = -1;
	return 0;
}

int CEpollServer::Live()
{
	return serverSocket;
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

int CEpollServer::PushEpoll(int socket, int event)
{
	struct epoll_event epollEvent;
	epollEvent.events = event;
	epollEvent.data.fd = socket;
	
	if (epoll_ctl(epollFD, EPOLL_CTL_ADD, socket, &epollEvent) < 0)
		throw CEpollServerException("PushEpoll Fail");

	if (socket != serverSocket)
	{
		clientLists.insert(std::pair<int, int>(socket, socket));
	}
	return 0;
}

int CEpollServer::PopEpoll(int socket)
{
	for (auto client : clientLists) {
		if (client.second == socket) {
			clientLists.erase(client.first);
			break;
		}
	}
	if (epoll_ctl(epollFD, EPOLL_CTL_DEL, socket, NULL) < 0)
		throw CEpollServerException("PopEpoll Fail");
	close(socket);
	return 0;
}

int CEpollServer::SearchClient(int deviceID)
{
	int clientSocket = clientLists[deviceID];

	if (clientSocket == 0)
	{
		return -1;
	}
	return clientSocket;
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