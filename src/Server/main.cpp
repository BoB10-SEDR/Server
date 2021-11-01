#include "stdafx.h"
#include "CMessage.h"
#include "CEpollServer.h"
#include "Function.h"
#include <algorithm>
#include "RestApiServer.h"
#include "CDatabase.h"

ST_ENV env;

void SendTest() {
	while (true)
	{
		sleep(0);
		char agentInfo[BUFFER_SIZE];
		char path[BUFFER_SIZE];

		int select;
		std::vector<ST_MONITOR_TARGET> logActiveLists;
		/*logActiveLists.push_back("/var/log/nginx/access.log");
		logActiveLists.push_back("/var/log/nginx/error.log");
		*/
		logActiveLists.push_back(ST_MONITOR_TARGET("ssh","/var/log/auth.log"));

		std::vector<ST_MONITOR_TARGET> logInactiveLists;
		logInactiveLists.push_back(ST_MONITOR_TARGET("ssh", "/var/log/auth.log"));

		printf("Agent : ");
		fgets(agentInfo, BUFFER_SIZE, stdin);
		*(agentInfo + (strlen(agentInfo) - 1)) = 0;

		printf("Opcode : ");
		scanf("%d", &select);
		printf("opcode : %d\n", select);
		while (getchar() != '\n');

		switch (select)
		{
		case 1:
			func::RequestProcessList(agentInfo);
			break;
		case 2:
			func::RequestFileDescriptorList(agentInfo, "1");
			break;
		case 3:
			func::RequestStartMonitoring(agentInfo, logActiveLists);
			break;
		case 4:
			func::RequestStopMonitoring(agentInfo, logInactiveLists);
			break;
		case 5:
			func::RequestDeviceInfo(agentInfo);
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

void SetLogger(std::string name, DWORD inputOption)
{
	std::tstring strModuleFile = core::GetFileName();
	std::tstring strModuleDir = core::ExtractDirectory(strModuleFile);
	std::tstring strModuleName = core::ExtractFileNameWithoutExt(strModuleFile);
	std::tstring strLogFile = strModuleDir + TEXT("/") + strModuleName + TEXT(".log");

	core::ST_LOG_INIT_PARAM_EX init;
	init.strLogFile = strLogFile;
	init.strID = TEXT(name);
	init.dwInputFlag = inputOption;
	init.dwOutputFlag = core::LOG_OUTPUT_FILE | core::LOG_OUTPUT_CONSOLE | core::LOG_OUTPUT_DBGWND;
	init.dwMaxFileSize = 10 * 1000 * 1000;
	init.dwMaxFileCount = 10;
	init.nLogRotation = core::LOG_ROTATION_SIZE;
	core::InitLog(init);
}

int main(int argc, char* argv[])
{
	if (argc != 5) {
		printf("syntax : ./server.out <socket port> <restapi port> <restapi thread count> <logger name>\n");
		printf("sample : ./server.out 12345 9080 2 logger-test\n");
		return -1;
	}

	env.socketPort = argv[1];
	env.apiPort = argv[2];
	env.thr = argv[3];
	env.loggerName = argv[4];

#ifdef _DEBUG
	SetLogger(env.loggerName, core::LOG_INFO | core::LOG_WARN | core::LOG_ERROR | core::LOG_DEBUG);
	core::Log_Info(TEXT("main.cpp - [%s]"), TEXT("Program is Debug Mode"));
#else
	SetLogger(env.loggerName, core::LOG_INFO | core::LOG_WARN | core::LOG_ERROR);
	core::Log_Info(TEXT("main.cpp - [%s]"), TEXT("Program is Release Mode"));
#endif
	
	core::Log_Info(TEXT("main.cpp - [%s]"), TEXT("Start Server Program!"));
	try
	{
		std::future<void> a = std::async(std::launch::async, &CMessage::Init, MessageManager());
		std::future<void> b = std::async(std::launch::async, &SendTest);

		Pistache::Port port(static_cast<uint16_t>(std::stol(env.apiPort)));

		int thr = std::stoi(env.thr);

		Pistache::Address addr(Pistache::Ipv4::any(), port);

		std::cout << "Cores = " << Pistache::hardware_concurrency() << std::endl;
		std::cout << "Using " << thr << " threads" << std::endl;

		CRestApiServer stats(addr);

		stats.Init(thr);
		stats.Start();
	}
	catch (std::exception& e)
	{
		core::Log_Error(TEXT("main.cpp - [%s]"), TEXT(e.what()));
	}
	core::Log_Info(TEXT("main.cpp - [%s]"), TEXT("Terminate Agent Program!"));

	return 0;
}