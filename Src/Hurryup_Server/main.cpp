#include "stdafx.h"
#include "CAgentServer.h"
#include "CRestApiServer.h"

ST_ENV env;

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

#ifdef _DEBUG
	SetLogger(env.loggerName, core::LOG_INFO | core::LOG_WARN | core::LOG_ERROR | core::LOG_DEBUG);
	core::Log_Info(TEXT("main.cpp - [%s]"), TEXT("Program is Debug Mode"));
#else
	SetLogger(env.loggerName, core::LOG_INFO | core::LOG_WARN | core::LOG_ERROR);
	core::Log_Info(TEXT("main.cpp - [%s]"), TEXT("Program is Release Mode"));
#endif

	env.socketPort = argv[1];
	env.apiPort = argv[2];
	env.thr = argv[3];
	env.loggerName = argv[4];

	RestApiServerManager()->Init();
	std::future<void> restApiServer = std::async(std::launch::async, &CRestApiServer::Start, RestApiServerManager());

	//AgentServerManager()->Init();
	//std::future<void> agentServer = std::async(std::launch::async, &CAgentServer::Start, AgentServerManager());

	return 0;
}