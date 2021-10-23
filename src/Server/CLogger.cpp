#include "CLogger.h"

CLogger::CLogger()
{
	logger = spdlog::daily_logger_mt("daily_logger", "logs/daily.txt", 2, 30);
	logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] [thread %t] [%@] [%!] %v");
	spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] [thread %t] [%@] [%!] %v");
}

CLogger::~CLogger()
{

}

CLogger* CLogger::GetInstance()
{
	static CLogger instance;
	return &instance;
}

void CLogger::Info(std::string msg)
{
	spdlog::info(msg);
	logger->info(msg);
}

void CLogger::Error(std::string msg)
{
	spdlog::error(msg);
	logger->error(msg);
}

void CLogger::Warn(std::string msg)
{
	spdlog::warn(msg);
	logger->warn(msg);
}

void CLogger::Critical(std::string msg)
{
	spdlog::critical(msg);
	logger->critical(msg);
}