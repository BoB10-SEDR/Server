#include "CMessage.h"
#include "CEpollServer.h"
#include "Function.h"

CMessage::CMessage()
{
}

CMessage::~CMessage()
{
	ServerManager()->End();
}

void CMessage::Init()
{
	LoggerManager()->Info("CMessage INIT");
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
	LoggerManager()->Info("Start SendMessage\n");
	while (1) {
		sleep(0);
		std::lock_guard<std::mutex> lock_guard(sendMessagemutex);

		if (sendMessage.empty())
			continue;

		ST_SERVER_PACKET_INFO* stServerPacketInfo = sendMessage.front();
		sendMessage.pop();

		std::tstring jsPacketSend;
		core::WriteJsonToString(stServerPacketInfo->stPacketInfo, jsPacketSend);
		
		ServerManager()->Send(stServerPacketInfo->agentInfo, jsPacketSend);
	}
}

void CMessage::ReceiveMessage()
{
	LoggerManager()->Info("Start ReceiveMessage\n");
	ServerManager()->Recv();
}

void CMessage::PushSendMessage(std::string agentInfo, PacketType type, PacketOpcode opcode, std::string message)
{
	sleep(0);
	std::lock_guard<std::mutex> lock_guard(sendMessagemutex);
	ST_SERVER_PACKET_INFO* stServerPacketInfo = new ST_SERVER_PACKET_INFO(agentInfo, new ST_PACKET_INFO(SERVER, AGENT, type, opcode, message));
	sendMessage.push(stServerPacketInfo);
}

void CMessage::PushReceiveMessage(std::string agentInfo, ST_PACKET_INFO* stPacketInfo)
{
	sleep(0);
	std::lock_guard<std::mutex> lock_guard(receiveMessagemutex);
	ST_SERVER_PACKET_INFO* stServerPacketInfo = new ST_SERVER_PACKET_INFO(agentInfo, stPacketInfo);
	receiveMessage.push(stServerPacketInfo);
}

void CMessage::MatchReceiveMessage()
{
	LoggerManager()->Info("Start MatchReceiveMessage\n");
	std::future<void> result;

	while (1)
	{
		sleep(0);
		std::lock_guard<std::mutex> lock_guard(receiveMessagemutex);

		if (receiveMessage.empty())
			continue;

		ST_SERVER_PACKET_INFO* stServerPacketInfo = receiveMessage.front();
		receiveMessage.pop();

		switch (stServerPacketInfo->stPacketInfo->opcode) {
		case OPCODE1:
			LoggerManager()->Info(StringFormatter("Opcode1 Result [%s] : %s\n", stServerPacketInfo->agentInfo.c_str(), stServerPacketInfo->stPacketInfo->data.c_str()));
			//result = std::async(std::launch::async, &CSample::Event1, sample);
			break;
		case OPCODE2:
			LoggerManager()->Info(StringFormatter("Opcode2 Result [%s] : %s\n", stServerPacketInfo->agentInfo.c_str(), stServerPacketInfo->stPacketInfo->data.c_str()));
			//result = std::async(std::launch::async, &CSample::Event2, sample);
			break;
		case OPCODE3:
			LoggerManager()->Info(StringFormatter("Opcode3 Result [%s] : %s\n", stServerPacketInfo->agentInfo.c_str(), stServerPacketInfo->stPacketInfo->data.c_str()));
			//result = std::async(std::launch::async, &CSample::Event3, sample, stServerPacketInfo->stPacketInfo->data.c_str());
			break;
		case PROCESS_LIST:
			result = std::async(std::launch::async, func::SaveProcessList, stServerPacketInfo->agentInfo, stServerPacketInfo->stPacketInfo->data.c_str());
			break;
		case FD_LIST:
			result = std::async(std::launch::async, func::SaveFileDescriptorList, stServerPacketInfo->agentInfo, stServerPacketInfo->stPacketInfo->data.c_str());
			break;
		case MONITOR_INFO:
			result = std::async(std::launch::async, func::SaveMonitoringInfo, stServerPacketInfo->agentInfo, stServerPacketInfo->stPacketInfo->data.c_str());
			break;
		case DEVICE_INFO:
			result = std::async(std::launch::async, func::SaveDeviceInfo, stServerPacketInfo->agentInfo, stServerPacketInfo->stPacketInfo->data.c_str());
			break;
		case MODULE_INFO:
			result = std::async(std::launch::async, func::SaveModuleInfo, stServerPacketInfo->agentInfo, stServerPacketInfo->stPacketInfo->data.c_str());
			break;
		default:
			LoggerManager()->Error(stServerPacketInfo->stPacketInfo->data.c_str());
			break;
		}
	}
}

CMessage* CMessage::GetInstance()
{
	static CMessage instance;
	return &instance;
}