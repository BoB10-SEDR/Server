#pragma once

#include <protocol.h>
#include <cppcore.h>

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

struct ST_SERVER_PACKET_INFO
{
	int agentSocket;
	ST_PACKET_INFO* stPacketInfo;

	ST_SERVER_PACKET_INFO() {}
	ST_SERVER_PACKET_INFO(int agentSocket_, ST_PACKET_INFO* stPacketInfo_) :agentSocket(agentSocket_), stPacketInfo(stPacketInfo_) {}
};