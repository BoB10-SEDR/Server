#include "stdafx.h"
#include "EpollServer.h"
#include <thread>


void SendTest(CEpollServer *server) {
	while (true)
	{
		printf("DeviceID : ");
		int deviceID;
		char message[1024];
		scanf("%d", &deviceID);
		printf("Message : ");
		scanf("%s", message);
		server->Send(deviceID, message);
	}
}

int main(int argc, char* argv[])
{
	CEpollServer server("127.0.0.1", "12345");
	std::vector<std::thread> works;

	server.Start();
	works.push_back(std::thread(&CEpollServer::Recv, &server));
	works.push_back(std::thread(&Test, &server));

	works[0].join();
	works[1].join();

	server.End();
	return 0;
}