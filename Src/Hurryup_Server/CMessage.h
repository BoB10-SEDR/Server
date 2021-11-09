#pragma once
#include "stdafx.h"
#include <queue>
#include <mutex>



class CMessage
{
private:
	std::queue<ST_SERVER_PACKET_INFO*> receiveMessage;
	std::queue<ST_SERVER_PACKET_INFO*> sendMessage;
	std::mutex receiveMessageMutex;
	std::mutex sendMessageMutex;

	CMessage();
	~CMessage();
public:
	static CMessage* GetInstance(void);;
	void PushSendMessage(int agentSocket, PacketType type, std::tstring opcode, std::tstring message);
	void PushReceiveMessage(int agentSocket, ST_PACKET_INFO* stPacketInfo);
	ST_SERVER_PACKET_INFO* PopSendMessage();
	ST_SERVER_PACKET_INFO* PopReceiveMessage();
};

inline CMessage* MessageManager()
{
	return CMessage::GetInstance();
}
