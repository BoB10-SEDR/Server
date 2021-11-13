#pragma once
#include "stdafx.h"
#include <queue>
#include <mutex>

class CMessage
{
private:
	std::queue<ST_MESSAGE_INFO*> receiveMessage;
	std::queue<ST_MESSAGE_INFO*> sendMessage;
	std::mutex receiveMessageMutex;
	std::mutex sendMessageMutex;

	CMessage();
	~CMessage();
public:
	static CMessage* GetInstance(void);;
	void PushSendMessage(int agentSocket, OPCODE opcode, std::tstring metaInfo);
	void PushReceiveMessage(int agentSocket, OPCODE opcode, std::tstring metaInfo);
	ST_MESSAGE_INFO* PopSendMessage();
	ST_MESSAGE_INFO* PopReceiveMessage();
};

inline CMessage* MessageManager()
{
	return CMessage::GetInstance();
}
