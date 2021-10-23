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
	std::map<std::string, int> agentInfoKeyLists;
	std::map<int, std::string> agentSocketKeyLists;

	CEpollServer();
	CEpollServer(std::string port);
	CEpollServer(std::string ip, std::string port);
	~CEpollServer();

	int CreateEpoll();
	int PushEpoll(std::string agentInfo, int agentSocket, int event);
	int PopEpoll(int agentSocket);
	int SearchAgent(std::string agentInfo);
	std::string SearchAgent(int agentSocket);
	std::string ConvertAgentInfo(struct sockaddr_in agentAddress);

public:
	static CEpollServer* GetInstance(void);
	static CEpollServer* GetInstance(std::string port);
	static CEpollServer* GetInstance(std::string ip, std::string port);

	int Start(int requestCount = 5);
	int Send(std::string deviceID, std::string message);
	int Recv();
	int End();
};

inline CEpollServer* ServerManager()
{
	return CEpollServer::GetInstance("12345");
}