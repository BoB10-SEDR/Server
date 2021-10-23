#include "CMessage.h"
#include "CEpollServer.h"

CMessage::CMessage()
{
}

CMessage::~CMessage()
{
	ServerManager()->End();
}

void CMessage::Init()
{
	ServerManager()->Start();
	std::future<void> c = std::async(std::launch::async, &CMessage::MatchReceiveMessage, this);
	std::future<void> b = std::async(std::launch::async, &CMessage::ReceiveMessage, this);
	std::future<void> a = std::async(std::launch::async, &CMessage::SendMessage, this);

	a.get();
	b.get();
	c.get();
}

void CMessage::SendMessage()
{
	printf("start SendMessage\n");
	while (1) {
		sleep(0);
		std::lock_guard<std::mutex> lock_guard(sendMessagemutex);

		if (sendMessage.empty())
			continue;

		ST_SERVER_PACKET_INFO* stServerPacketInfo = sendMessage.front();
		sendMessage.pop();

		std::tstring jsPacketSend;
		core::WriteJsonToString(stServerPacketInfo->stPacketInfo, jsPacketSend);

		ServerManager()->Send(stServerPacketInfo->deviceID, jsPacketSend);
	}
}

void CMessage::ReceiveMessage()
{
	printf("start ReceiveMessage\n");
	ServerManager()->Recv();
}

void CMessage::PushSendMessage(int deviceID, PacketType type, PacketOpcode opcode, std::string message)
{
	sleep(0);
	std::lock_guard<std::mutex> lock_guard(sendMessagemutex);
	ST_SERVER_PACKET_INFO* stServerPacketInfo = new ST_SERVER_PACKET_INFO(deviceID, new ST_PACKET_INFO(SERVER, AGENT, type, opcode, message));
	sendMessage.push(stServerPacketInfo);
}

void CMessage::PushReceiveMessage(int deviceID, ST_PACKET_INFO* stPacketInfo)
{
	sleep(0);
	std::lock_guard<std::mutex> lock_guard(receiveMessagemutex);
	ST_SERVER_PACKET_INFO* stServerPacketInfo = new ST_SERVER_PACKET_INFO(deviceID, stPacketInfo);
	receiveMessage.push(stServerPacketInfo);
}

void CMessage::MatchReceiveMessage()
{
	printf("start MatchReceiveMessage\n");
	std::future<void> result;

	while (1)
	{
		sleep(0);
		std::lock_guard<std::mutex> lock_guard(receiveMessagemutex);

		if (receiveMessage.empty())
			continue;

		ST_SERVER_PACKET_INFO* stServerPacketInfo = receiveMessage.front();
		receiveMessage.pop();
		printf("Match [%d] %d %d\n", receiveMessage.size(), stServerPacketInfo->deviceID, stServerPacketInfo->stPacketInfo->opcode);

		switch (stServerPacketInfo->stPacketInfo->opcode) {
		case OPCODE1:
			printf("OPcode1 : %d %d %s\n", stServerPacketInfo->deviceID, stServerPacketInfo->stPacketInfo->opcode, stServerPacketInfo->stPacketInfo->data.c_str());
			//result = std::async(std::launch::async, &CSample::Event1, sample);
			break;
		case OPCODE2:
			printf("OPcode2 : %d %d %s\n", stServerPacketInfo->deviceID, stServerPacketInfo->stPacketInfo->opcode, stServerPacketInfo->stPacketInfo->data.c_str());
			//result = std::async(std::launch::async, &CSample::Event2, sample);
			break;
		case OPCODE3:
			printf("OPcode3 : %d %d %s\n", stServerPacketInfo->deviceID, stServerPacketInfo->stPacketInfo->opcode, stServerPacketInfo->stPacketInfo->data.c_str());
			//result = std::async(std::launch::async, &CSample::Event3, sample, stServerPacketInfo->stPacketInfo->data.c_str());
			break;
		default:
			printf("Message Error : %d %s", stServerPacketInfo->deviceID, stServerPacketInfo->stPacketInfo->data.c_str());
			break;
		}
	}
}

CMessage* CMessage::GetInstance()
{
	static CMessage instance;
	return &instance;
}