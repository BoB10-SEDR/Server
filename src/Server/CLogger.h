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

	void Info(std::string message);
	void Error(std::string message);
	void Warn(std::string message);
	void Critical(std::string message);
};

inline CLogger* LoggerManager()
{
	return CLogger::GetInstance();
}
