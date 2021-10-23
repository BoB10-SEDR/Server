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

	void ReceiveMessage();	//������Ʈ���� ���� �޽����� �����ϴ� ���
	void SendMessage();		//������Ʈ�� �޽����� ������ ���
	void MatchReceiveMessage();	//���� �޽���ť���� �޽����� �о�, ������ Ư�� ��ɰ� ��Ī �����ִ� ���
public:
	~CMessage();

	static CMessage* GetInstance(void);
	void Init();
	void PushSendMessage(int deviceID, PacketType type, PacketOpcode opcode, std::string message);	//���� �޽����� sendMessage�� ť�� �߰�
	void PushReceiveMessage(int deviceID, ST_PACKET_INFO* stPacketInfo);	//���� �޽����� sendMessage�� ť�� �߰�
};

inline CMessage* MessageManager()
{
	return CMessage::GetInstance();
}