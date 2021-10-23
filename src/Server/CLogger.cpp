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

void CLogger::Info(std::string message)
{
	spdlog::info(message);
	logger->info(message);
}

void CLogger::Error(std::string message)
{
	spdlog::error(message);
	logger->error(message);
}

void CLogger::Warn(std::string message)
{
	spdlog::warn(message);
	logger->warn(message);
}

void CLogger::Critical(std::string message)
{
	spdlog::critical(message);
	logger->critical(message);
}

CLogger* CLogger::GetInstance()
{
	static CLogger instance;
	return &instance;
}