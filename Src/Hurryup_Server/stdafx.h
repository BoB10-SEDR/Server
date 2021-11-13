#pragma once
#include <protocol.h>
#include <cppcore.h>
#include "new_packet.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <future>
#include <nlohmann/json.hpp>

struct ST_ENV
{
	std::tstring socketPort;
	std::tstring apiPort;
	std::tstring thr;
	std::tstring loggerName;
};
