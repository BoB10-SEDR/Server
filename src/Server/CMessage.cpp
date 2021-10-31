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
	core::Log_Debug(TEXT("CMessage.cpp - [%s]"), TEXT("Init"));

	ServerManager()->Start();

	std::future<void> a = std::async(std::launch::async, &CMessage::MatchReceiveMessage, this);
	std::future<void> b = std::async(std::launch::async, &CMessage::ReceiveMessage, this);
	std::future<void> c = std::async(std::launch::async, &CMessage::SendMessage, this);
}

void CMessage::SendMessage()
{
	core::Log_Debug(TEXT("CMessage.cpp - [%s]"), TEXT("Working SendMessage In Thread"));
	ST_SERVER_PACKET_INFO* stServerPacketInfo;

	while (1) {
		sleep(0);
		{
			std::lock_guard<std::mutex> lock_guard(sendMessageMutex);

			if (sendMessage.empty())
				continue;

			stServerPacketInfo = sendMessage.front();
			sendMessage.pop();
		}

		std::tstring jsPacketSend;
		core::WriteJsonToString(stServerPacketInfo->stPacketInfo, jsPacketSend);
		ServerManager()->Send(stServerPacketInfo->agentInfo, jsPacketSend + "END");
		core::Log_Debug(TEXT("CMessage.cpp - [%s] : %s -> %s"), TEXT("Send Message"), TEXT(stServerPacketInfo->agentInfo.c_str()), TEXT(jsPacketSend.c_str()));
		free(stServerPacketInfo->stPacketInfo);
		free(stServerPacketInfo);
	}
}

void CMessage::ReceiveMessage()
{
	core::Log_Debug(TEXT("CMessage.cpp - [%s]"), TEXT("Working ReceiveMessage In Thread"));
	ServerManager()->Recv();
}

void CMessage::PushSendMessage(std::tstring agentInfo, PacketType type, PacketOpcode opcode, std::tstring message)
{
	sleep(0);
	std::lock_guard<std::mutex> lock_guard(sendMessageMutex);
	ST_SERVER_PACKET_INFO* stServerPacketInfo = new ST_SERVER_PACKET_INFO(agentInfo, new ST_PACKET_INFO(SERVER, AGENT, type, opcode, message));
	sendMessage.push(stServerPacketInfo);
}

void CMessage::PushReceiveMessage(std::tstring agentInfo, ST_PACKET_INFO* stPacketInfo)
{
	sleep(0);
	std::lock_guard<std::mutex> lock_guard(receiveMessageMutex);
	ST_SERVER_PACKET_INFO* stServerPacketInfo = new ST_SERVER_PACKET_INFO(agentInfo, stPacketInfo);
	receiveMessage.push(stServerPacketInfo);
}

void CMessage::MatchReceiveMessage()
{
	core::Log_Debug(TEXT("CMessage.cpp - [%s]"), TEXT("Working MatchReceiveMessage In Thread"));
	std::future<void> result;
	ST_SERVER_PACKET_INFO* stServerPacketInfo;

	while (1)
	{
		sleep(0);
		{
			std::lock_guard<std::mutex> lock_guard(receiveMessageMutex);

			if (receiveMessage.empty())
				continue;

			stServerPacketInfo = receiveMessage.front();
			receiveMessage.pop();
		}

		switch (stServerPacketInfo->stPacketInfo->opcode) {
		case PROCESS_LIST:
			result = std::async(std::launch::async, func::ResponseProcessList, stServerPacketInfo->agentInfo, stServerPacketInfo->stPacketInfo->data);
			break;
		case FD_LIST:
			result = std::async(std::launch::async, func::ResponseFileDescriptorList, stServerPacketInfo->agentInfo, stServerPacketInfo->stPacketInfo->data);
			break;
		case MONITOR_RESULT:
			result = std::async(std::launch::async, func::ResponseMonitoringResult, stServerPacketInfo->agentInfo, stServerPacketInfo->stPacketInfo->data);
			break;
		case MONITOR_INFO:
			result = std::async(std::launch::async, func::ResponseMonitoringLog, stServerPacketInfo->agentInfo, stServerPacketInfo->stPacketInfo->data);
			break;
		case DEVICE_INFO:
			result = std::async(std::launch::async, func::SaveDeviceInfo, stServerPacketInfo->agentInfo, stServerPacketInfo->stPacketInfo->data);
			break;
		case MODULE_INFO:
			result = std::async(std::launch::async, func::SaveModuleInfo, stServerPacketInfo->agentInfo, stServerPacketInfo->stPacketInfo->data);
			break;
		case POLICY_STATE:
			result = std::async(std::launch::async, func::SavePolicyStatus, stServerPacketInfo->agentInfo, stServerPacketInfo->stPacketInfo->data);
			break;
		case CHECK_STATE:
			result = std::async(std::launch::async, func::SaveCheckStatus, stServerPacketInfo->agentInfo, stServerPacketInfo->stPacketInfo->data);
			break;
		case MESSAGE:
			result = std::async(std::launch::async, func::SaveMessage, stServerPacketInfo->agentInfo, stServerPacketInfo->stPacketInfo->data);
			break;
		default:
			core::Log_Error(TEXT("CMessage.cpp - [%s] : %s "), TEXT("Packet Type Error"), TEXT(stServerPacketInfo->stPacketInfo->data.c_str()));
			break;
		}
	}
}

CMessage* CMessage::GetInstance()
{
	static CMessage instance;
	return &instance;
}