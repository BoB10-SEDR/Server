#include "CMessage.h"

CMessage::CMessage()
{
}

CMessage::~CMessage()
{
}

void CMessage::PushSendMessage(int agentSocket, PacketType type, std::tstring opcode, std::tstring message)
{
	std::lock_guard<std::mutex> lock_guard(sendMessageMutex);
	ST_SERVER_PACKET_INFO* stServerPacketInfo = new ST_SERVER_PACKET_INFO(agentSocket, new ST_PACKET_INFO(SERVER, AGENT, type, opcode, message));
	sendMessage.push(stServerPacketInfo);
}

void CMessage::PushReceiveMessage(int agentSocket, ST_PACKET_INFO* stPacketInfo)
{
	std::lock_guard<std::mutex> lock_guard(receiveMessageMutex);
	ST_SERVER_PACKET_INFO* stServerPacketInfo = new ST_SERVER_PACKET_INFO(agentSocket, stPacketInfo);
	receiveMessage.push(stServerPacketInfo);
}

ST_SERVER_PACKET_INFO* CMessage::PopSendMessage()
{
	std::lock_guard<std::mutex> lock_guard(sendMessageMutex);

	ST_SERVER_PACKET_INFO* stServerPacketInfo = new ST_SERVER_PACKET_INFO();

	if (sendMessage.empty())
		return NULL;

	stServerPacketInfo = sendMessage.front();
	sendMessage.pop();

	return stServerPacketInfo;
}

ST_SERVER_PACKET_INFO* CMessage::PopReceiveMessage()
{
	std::lock_guard<std::mutex> lock_guard(receiveMessageMutex);

	ST_SERVER_PACKET_INFO* stServerPacketInfo = new ST_SERVER_PACKET_INFO();

	if (receiveMessage.empty())
		return NULL;

	stServerPacketInfo = receiveMessage.front();
	receiveMessage.pop();

	return stServerPacketInfo;
}

CMessage* CMessage::GetInstance()
{
	static CMessage instance;
	return &instance;
}