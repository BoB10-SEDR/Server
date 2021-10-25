#include "stdafx.h"
#include "CMessage.h"
#include "CEpollServer.h"
#include "Function.h"

void SendTest() {
	while (true)
	{
		sleep(0);
		char agentInfo[BUFFER_SIZE];
		int select;
		std::vector<std::string> logLists;
		logLists.push_back("/path/logs/1");
		logLists.push_back("/path/logs/2");
		logLists.push_back("/path/logs/3");
		
		printf("Agent : ");
		fgets(agentInfo, BUFFER_SIZE, stdin);
		*(agentInfo + (strlen(agentInfo) - 1)) = 0;

		printf("Opcode : ");
		scanf("%d", &select);
		printf("opcode : %d\n", select);

		switch (select)
		{
		case 1:
			func::GetProcessList(agentInfo);
			break;
		case 2:
			func::GetFileDescriptorList(agentInfo);
			break;
		case 3:
			func::StartMonitoring(agentInfo, logLists);
			break;
		case 4:
			func::StopMonitoring(agentInfo, logLists);
			break;
		case 5:
			func::GetDeviceInfo(agentInfo);
			break;
		case 6:
			func::GetModuleInfo(agentInfo);
			break;
		case 7:
			func::ActivatePolicy(agentInfo, 1, "Policy", "1.1");
			break;
		case 8:
			func::InactivatePolicy(agentInfo, 1, "Policy", "1.1");
			break;
		case 9:
			func::ActivateCheck(agentInfo, 1, "Check");
			break;
		default:
			break;
		}
	}
}

int main(int argc, char* argv[])
{
	try
	{
		LoggerManager()->Info("Start Server Program!");

		std::future<void> a = std::async(std::launch::async, &CMessage::Init, MessageManager());
		std::future<void> b = std::async(std::launch::async, &SendTest);
	}
	catch (std::exception& e)
	{
		LoggerManager()->Error(e.what());
	}
	LoggerManager()->Info("Terminate server Program!");

	return 0;
}

