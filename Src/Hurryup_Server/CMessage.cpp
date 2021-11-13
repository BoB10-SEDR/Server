#include "CMessage.h"

CMessage::CMessage()
{
}

CMessage::~CMessage()
{
}

void CMessage::PushSendMessage(int agentSocket, OPCODE opcode, std::tstring metaInfo)
{
	std::lock_guard<std::mutex> lock_guard(sendMessageMutex);
	ST_MESSAGE_INFO* stMessageInfo = new ST_MESSAGE_INFO(agentSocket, opcode, metaInfo);
	sendMessage.push(stMessageInfo);
}

void CMessage::PushReceiveMessage(int agentSocket, OPCODE opcode, std::tstring metaInfo)
{
	std::lock_guard<std::mutex> lock_guard(receiveMessageMutex);
	ST_MESSAGE_INFO* stMessageInfo = new ST_MESSAGE_INFO(agentSocket, opcode, metaInfo);
	receiveMessage.push(stMessageInfo);
}

ST_MESSAGE_INFO* CMessage::PopSendMessage()
{
	std::lock_guard<std::mutex> lock_guard(sendMessageMutex);

	ST_MESSAGE_INFO* stMessageInfo = new ST_MESSAGE_INFO();

	if (sendMessage.empty())
		return NULL;

	stMessageInfo = sendMessage.front();
	sendMessage.pop();

	return stMessageInfo;
}

ST_MESSAGE_INFO* CMessage::PopReceiveMessage()
{
	std::lock_guard<std::mutex> lock_guard(receiveMessageMutex);

	ST_MESSAGE_INFO* stMessageInfo = new ST_MESSAGE_INFO();

	if (receiveMessage.empty())
		return NULL;

	stMessageInfo = receiveMessage.front();
	receiveMessage.pop();

	return stMessageInfo;
}

CMessage* CMessage::GetInstance()
{
	static CMessage instance;
	return &instance;
}