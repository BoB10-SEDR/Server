#pragma once
#include <queue>
#include <protocol.h>
#include <mutex>
#include <future>

struct ST_SERVER_PACKET_INFO
{
	int deviceID;
	ST_PACKET_INFO* stPacketInfo;

	ST_SERVER_PACKET_INFO(int deviceID_, ST_PACKET_INFO* stPacketInfo_) :deviceID(deviceID_), stPacketInfo(stPacketInfo_) {}
};

class CMessage
{
private:
	std::queue<ST_SERVER_PACKET_INFO*> receiveMessage;
	std::queue<ST_SERVER_PACKET_INFO*> sendMessage;
	std::mutex receiveMessagemutex;
	std::mutex sendMessagemutex;

	CMessage();

	void ReceiveMessage();	//에이전트에서 받은 메시지를 수신하는 기능
	void SendMessage();		//에이전트로 메시지를 보내는 기능
	void MatchReceiveMessage();	//수신 메시지큐에서 메시지를 읽어, 서버의 특정 기능과 매칭 시켜주는 기능
public:
	~CMessage();

	static CMessage* GetInstance(void);
	void Init();
	void PushSendMessage(int deviceID, PacketType type, PacketOpcode opcode, std::string message);	//보낼 메시지를 sendMessage에 큐에 추가
	void PushReceiveMessage(int deviceID, ST_PACKET_INFO* stPacketInfo);	//보낼 메시지를 sendMessage에 큐에 추가
};

inline CMessage* MessageManager()
{
	return CMessage::GetInstance();
}