#include "EpollServer.h"

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
	//����ó�� �ʿ�
	serverSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0)
	{
		printf("Socker Error\n");
	}

	//����ó�� �ʿ�
	int error =bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	if (error < 0)
	{
		printf("Bind Error\n");
	}

	//����ó�� �ʿ�
	error = listen(serverSocket, requestCount);
	if (error < 0)
	{
		printf("listen Error\n");
	}

	//����ó�� �ʿ�
	CEpollServer::PushEpoll(serverSocket, EPOLLIN);

	printf("server Start\n");

	return 0;
}

int CEpollServer::Send(int deviceID, std::string message)
{
	//����ó�� �ʿ�
	int clientSocket = SearchClient(deviceID);
	write(clientSocket, message.c_str(), message.length());
	return 0;
}

int CEpollServer::Recv()
{
	int timeout = -1;
	while (true)
	{
		int eventCount = epoll_wait(epollFD, epollEvents, EPOLL_SIZE, timeout);
		printf("event count[%d]\n", eventCount);

		if (eventCount < 0)
		{
			printf("epoll_wait() error [%d]\n", eventCount);
			return 0;
		}

		for (int i = 0; i < eventCount; i++)
		{
			if (epollEvents[i].data.fd == serverSocket)
			{
				int            clientSocket;
				int            clientLength;
				struct sockaddr_in    clientAddress;

				printf("User Accept\n");
				clientLength = sizeof(clientAddress);
				clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, (socklen_t*)&clientLength);

				// client fd Non-Blocking Socket���� ����. Edge Trigger ����ϱ� ���� ����. 
				int flags = fcntl(clientSocket, F_GETFL);
				flags |= O_NONBLOCK;

				if (fcntl(clientSocket, F_SETFL, flags) < 0)
				{
					printf("client_fd[%d] fcntl() error\n", clientSocket);
					return 0;
				}

				if (clientSocket < 0)
				{
					printf("accept() error [%d]\n", clientSocket);
					continue;
				}

				PushEpoll(clientSocket, EPOLLIN | EPOLLET);
			}
			else
			{
				// epoll�� ��� �� Ŭ���̾�Ʈ���� send data ó��
				int clientSocket = epollEvents[i].data.fd;
				int messageLength;
				char message[BUFFER_SIZE];

				messageLength = read(clientSocket, &message, BUFFER_SIZE);

				if (messageLength <= 0)
				{
					// Ŭ���̾�Ʈ ���� ���� ��û
					printf("Client Disconnect [%d]\n", clientSocket);
					PopEpoll(clientSocket);
				}
				else
				{
					// ���� ���� ��û�� �ƴ� ��� ��û�� ���뿡 ���� ó��.
					printf("Recv Data from [%d]\n", clientSocket);
					printf("message : %s\n", message);
				}
			}
		}
	}
}

int CEpollServer::End()
{
	//epollFD�� ã�Ƽ� �����ؾ��Ѵ�.
	//for (int i = 0; i < EPOLL_SIZE; i++)
	//{
	//	close(epollEvents->data.fd);
	//}
	clientLists.clear();
	free(epollEvents);
	close(epollFD);
	close(serverSocket);
	return 0;
}

int CEpollServer::CreateEpoll()
{
	epollFD = epoll_create(EPOLL_SIZE);
	epollEvents = (struct epoll_event*)malloc(sizeof(struct epoll_event) * EPOLL_SIZE);
	memset(epollEvents, 0, sizeof(struct epoll_event) * EPOLL_SIZE);
	return 0;
}

int CEpollServer::PushEpoll(int socket, int event)
{
	struct epoll_event epollEvent;
	epollEvent.events = event;
	epollEvent.data.fd = socket;
	//���� ó�� �ʿ�
	epoll_ctl(epollFD, EPOLL_CTL_ADD, socket, &epollEvent);

	if (socket != serverSocket)
	{
		clientLists.insert(std::pair<int, int>(socket, socket));
	}
	return 0;
}

int CEpollServer::PopEpoll(int socket)
{
	//���� ó�� �ʿ�
	for (auto client : clientLists) {
		if (client.second == socket) {
			clientLists.erase(client.first);
			break;
		}
	}
	epoll_ctl(epollFD, EPOLL_CTL_DEL, socket, NULL);
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