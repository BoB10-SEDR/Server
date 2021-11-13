#pragma once
#include "stdafx.h"
#include "CServer.h"
#include "CApi.h"
#include "CAgentApi.h"

#include <unistd.h>
#include <fcntl.h> 
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <map>

#define EPOLL_SIZE 100
#define BUFFER_SIZE 1024

class CAgentServer : public CServer
{
private:
	std::map<OPCODE, void(CAgentApi::*)(int, std::tstring)> router;
	std::vector<CApi<std::map<OPCODE, void(CAgentApi::*)(int, std::tstring)>>*> apiList;

	int serverSocket, epollFD;
	struct sockaddr_in serverAddress;
	struct epoll_event* epollEvents;
	std::map<int, std::tstring> agentMessageBuffers;

	CAgentServer();
	CAgentServer(std::tstring port);
	CAgentServer(std::tstring ip, std::tstring port);
	~CAgentServer();

	int CreateEpoll();
	int PushEpoll(int agentSocket, int event);
	int PopEpoll(int agentSocket);
	int SearchAgent(int agentSocket);
public:
	static CAgentServer* GetInstance(void);
	void Send();
	void Recv();
	void FileSend();
	void MatchMessage();

	virtual void Init();
	virtual void Start();
	virtual void End();
	virtual void SetupRoutes();
};


inline CAgentServer* AgentServerManager()
{
	return CAgentServer::GetInstance();
}
