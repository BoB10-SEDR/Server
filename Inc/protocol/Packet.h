#pragma once

struct PACKET_SUPER
{
	int nID;
	int nSize;
	PACKET_SUPER(void)
	{}
	PACKET_SUPER(int id, int size)
		: nID(id), nSize(size)
	{}
};

struct REQ_LOGIN : public PACKET_SUPER
{
	char szName[21];
	char szPass[21];
	REQ_LOGIN(void)
		: PACKET_SUPER(1, sizeof(*this))
	{}
};

struct CMD_LOGIN : public PACKET_SUPER
{
	int nResult;	// 0:OK, 1:NOK
	CMD_LOGIN(void)
		: PACKET_SUPER(1, sizeof(*this))
	{}
};

void test();