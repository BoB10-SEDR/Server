#include "Cutils.h"

std::string Cutils::GetTimeStamp()
{
	time_t curTime = time(NULL);
	char timestamp[20];
	struct tm* a = localtime(&curTime);

	sprintf(timestamp, "%04d-%02d-%02d %02d:%02d:%02d", 1900 + a->tm_year, a->tm_mon + 1, a->tm_mday, a->tm_hour, a->tm_min, a->tm_sec);
	return timestamp;
}