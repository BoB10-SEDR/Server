#pragma once
#include "stdafx.h"
#include <unistd.h>
#include <fcntl.h> 
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <map>

#define EPOLL_SIZE 100
#define BUFFER_SIZE 1024

class CEpollServer
{
private:
	int serverSocket, epollFD;
	struct sockaddr_in serverAddress;
	struct epoll_event* epollEvents;
	std::map<std::tstring, int> agentInfoKeyLists;
	std::map<int, std::tstring> agentSocketKeyLists;
	std::map<int, std::tstring> agentMessageBuffers;

	CEpollServer();
	CEpollServer(std::tstring port);
	CEpollServer(std::tstring ip, std::tstring port);
	~CEpollServer();

	int CreateEpoll();
	int PushEpoll(std::tstring agentInfo, int agentSocket, int event);
	int PopEpoll(int agentSocket);
	int SearchAgent(std::tstring agentInfo);
	std::tstring SearchAgent(int agentSocket);
	std::tstring ConvertAgentInfo(struct sockaddr_in agentAddress);

public:
	static CEpollServer* GetInstance(void);
	static CEpollServer* GetInstance(std::tstring port);
	static CEpollServer* GetInstance(std::tstring ip, std::tstring port);

	int Start(int requestCount = 5);
	int Send(std::tstring deviceID, std::tstring message);
	int Recv();
	int End();
};

inline CEpollServer* ServerManager()
{
	return CEpollServer::GetInstance("12345");
}