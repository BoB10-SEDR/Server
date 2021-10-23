#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/daily_file_sink.h>


class CLogger
{
private:
	std::shared_ptr<spdlog::logger> logger;

	CLogger();
	~CLogger();
public:
	static CLogger* GetInstance(void);
	void Info(std::string msg);
	void Error(std::string msg);
	void Warn(std::string msg);
	void Critical(std::string msg);
};

inline CLogger* LoggerManager()
{
	return CLogger::GetInstance();
}

