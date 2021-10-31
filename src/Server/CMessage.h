#pragma once
#include "stdafx.h"
#include <queue>
#include <mutex>
#include <future>

struct ST_SERVER_PACKET_INFO
{
	std::tstring agentInfo;
	ST_PACKET_INFO* stPacketInfo;

	ST_SERVER_PACKET_INFO(std::tstring agentInfo_, ST_PACKET_INFO* stPacketInfo_) :agentInfo(agentInfo_), stPacketInfo(stPacketInfo_) {}
};

class CMessage
{
private:
	std::queue<ST_SERVER_PACKET_INFO*> receiveMessage;
	std::queue<ST_SERVER_PACKET_INFO*> sendMessage;
	std::mutex receiveMessageMutex;
	std::mutex sendMessageMutex;

	CMessage();
	~CMessage();

	void ReceiveMessage();	//에이전트에서 받은 메시지를 수신하는 기능
	void SendMessage();		//에이전트로 메시지를 보내는 기능
	void MatchReceiveMessage();	//수신 메시지큐에서 메시지를 읽어, 서버의 특정 기능과 매칭 시켜주는 기능
public:
	static CMessage* GetInstance(void);
	void Init();
	void PushSendMessage(std::tstring agentInfo, PacketType type, PacketOpcode opcode, std::tstring message);	//보낼 메시지를 sendMessage에 큐에 추가
	void PushReceiveMessage(std::tstring agentInfo, ST_PACKET_INFO* stPacketInfo);	//받은 메시지를 receiveMessage에 큐에 추가
};

inline CMessage* MessageManager()
{
	return CMessage::GetInstance();
}