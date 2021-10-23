#include "stdafx.h"
#include "CMessage.h"
#include "CEpollServer.h"

void SendTest() {
	while (true)
	{
		sleep(0);
		char message[BUFFER_SIZE];

		printf("Message : ");
		fgets(message, BUFFER_SIZE, stdin);
		*(message + (strlen(message) - 1)) = 0;

		MessageManager()->PushSendMessage(message, REQUEST, OPCODE2, "Data Return");
	}
}

int main(int argc, char* argv[])
{
	try
	{
		LoggerManager()->Info("Start Server Program!");

		std::future<void> a = std::async(std::launch::async, &CMessage::Init, MessageManager());
		std::future<void> b = std::async(std::launch::async, &SendTest);
	}
	catch (std::exception& e)
	{
		LoggerManager()->Error(e.what());
	}
	LoggerManager()->Info("Terminate server Program!");

	return 0;
}

