﻿#include "stdafx.h"
#include "EpollServer.h"
#include <thread>

void SendTest(CEpollServer *server) {
	while (true)
	{
		int deviceID;
		char message[1024];

		printf("DeviceID : ");
		scanf("%d", &deviceID);

		while (getchar() != '\n');

		printf("Message : ");
		fgets(message, BUFFER_SIZE, stdin);

		ST_PACKET_INFO stPacketSend(SERVER, AGENT, REQUEST, OPCODE1, message);

		std::tstring jsPacketSend;
		core::WriteJsonToString(&stPacketSend, jsPacketSend);

		server->Send(deviceID, jsPacketSend);
	}
}

int main(int argc, char* argv[])
{
	CEpollServer server("127.0.0.1", "12345");
	std::vector<std::thread> works;

	try
	{
		server.Start();
		works.push_back(std::thread(&CEpollServer::Recv, &server));
		works.push_back(std::thread(&SendTest, &server));

		works[0].join();
		works[1].join();

		server.End();
	}
	catch (std::exception& e)
	{
		printf("%s\n", e.what());
	}

	return 0;
}