#pragma once
#include <string>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> 
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <map>
#include "EpollServerException.h"

#define EPOLL_SIZE 100
#define BUFFER_SIZE 1024

class CEpollServer
{
private:
	int serverSocket, epollFD;
	struct sockaddr_in serverAddress;
	struct epoll_event* epollEvents;
	std::map<int, int> clientLists;

	int CreateEpoll();
	int PushEpoll(int socket, int event);
	int PopEpoll(int socket);
	int SearchClient(int clientID);

public:
	CEpollServer();
	CEpollServer(std::string port);
	CEpollServer(std::string ip, std::string port);
	~CEpollServer();

	int Start(int requestCount = 5);
	int Send(int deviceID, std::string message);
	int Recv();
	int End();
};

