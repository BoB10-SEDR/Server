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

	void ReceiveMessage();	//������Ʈ���� ���� �޽����� �����ϴ� ���
	void SendMessage();		//������Ʈ�� �޽����� ������ ���
	void MatchReceiveMessage();	//���� �޽���ť���� �޽����� �о�, ������ Ư�� ��ɰ� ��Ī �����ִ� ���
public:
	static CMessage* GetInstance(void);
	void Init();
	void PushSendMessage(std::tstring agentInfo, PacketType type, PacketOpcode opcode, std::tstring message);	//���� �޽����� sendMessage�� ť�� �߰�
	void PushReceiveMessage(std::tstring agentInfo, ST_PACKET_INFO* stPacketInfo);	//���� �޽����� receiveMessage�� ť�� �߰�
};

inline CMessage* MessageManager()
{
	return CMessage::GetInstance();
}