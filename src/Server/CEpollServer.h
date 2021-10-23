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
#include "stdafx.h"

#define EPOLL_SIZE 100
#define BUFFER_SIZE 1024

class CEpollServer
{
private:
	int serverSocket, epollFD;
	struct sockaddr_in serverAddress;
	struct epoll_event* epollEvents;
	std::map<int, int> clientLists;

	CEpollServer();
	CEpollServer(std::string port);
	CEpollServer(std::string ip, std::string port);
	~CEpollServer();

	int CreateEpoll();
	int PushEpoll(int socket, int event);
	int PopEpoll(int socket);
	int SearchClient(int clientID);

public:
	static CEpollServer* GetInstance(void);
	static CEpollServer* GetInstance(std::string port);
	static CEpollServer* GetInstance(std::string ip, std::string port);

	int Start(int requestCount = 5);
	int Send(int deviceID, std::string message);
	int Live();
	int Recv();
	int End();
};

inline CEpollServer* ServerManager()
{
	return CEpollServer::GetInstance("12345");
}