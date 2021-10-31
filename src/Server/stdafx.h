#pragma once
#include <cstdio>
#include <protocol.h>
#include <memory>
#include <string>
#include <string.h>

struct ST_ENV
{
	std::tstring socketPort;
	std::tstring apiPort;
	std::tstring thr;
	std::tstring loggerName;
};