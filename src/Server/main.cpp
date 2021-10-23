#include "stdafx.h"
#include "CMessage.h"
#include "CEpollServer.h"
#include <signal.h>

void interruptHandler(int sig) {
	delete MessageManager();
}

void SendTest() {
	int i = 0;
	std::vector <ST_PACKET_INFO*> test;
	test.push_back(new ST_PACKET_INFO(SERVER, AGENT, REQUEST, OPCODE1, "Data Reset"));
	test.push_back(new ST_PACKET_INFO(SERVER, AGENT, REQUEST, OPCODE2, "Data Return"));
	test.push_back(new ST_PACKET_INFO(SERVER, AGENT, REQUEST, OPCODE3, "message1"));
	test.push_back(new ST_PACKET_INFO(SERVER, AGENT, REQUEST, OPCODE2, "Data Return"));
	test.push_back(new ST_PACKET_INFO(SERVER, AGENT, REQUEST, OPCODE2, "Data Return"));
	test.push_back(new ST_PACKET_INFO(SERVER, AGENT, REQUEST, OPCODE1, "Data Reset"));
	test.push_back(new ST_PACKET_INFO(SERVER, AGENT, REQUEST, OPCODE2, "Data Return"));
	test.push_back(new ST_PACKET_INFO(SERVER, AGENT, REQUEST, OPCODE3, "message2"));
	test.push_back(new ST_PACKET_INFO(SERVER, AGENT, REQUEST, OPCODE2, "Data Return"));
	test.push_back(new ST_PACKET_INFO(SERVER, AGENT, REQUEST, OPCODE2, "Data Return"));
	test.push_back(new ST_PACKET_INFO(SERVER, AGENT, REQUEST, OPCODE1, "Data Reset"));
	test.push_back(new ST_PACKET_INFO(SERVER, AGENT, REQUEST, OPCODE2, "Data Return"));
	test.push_back(new ST_PACKET_INFO(SERVER, AGENT, REQUEST, OPCODE3, "message3"));
	test.push_back(new ST_PACKET_INFO(SERVER, AGENT, REQUEST, OPCODE2, "Data Return"));
	test.push_back(new ST_PACKET_INFO(SERVER, AGENT, REQUEST, OPCODE2, "Data Return"));
	test.push_back(new ST_PACKET_INFO(SERVER, AGENT, REQUEST, OPCODE1, "Data Reset"));
	test.push_back(new ST_PACKET_INFO(SERVER, AGENT, REQUEST, OPCODE2, "Data Return"));
	test.push_back(new ST_PACKET_INFO(SERVER, AGENT, REQUEST, OPCODE3, "message4"));
	test.push_back(new ST_PACKET_INFO(SERVER, AGENT, REQUEST, OPCODE2, "Data Return"));
	test.push_back(new ST_PACKET_INFO(SERVER, AGENT, REQUEST, OPCODE2, "Data Return"));

	while (true)
	{
		sleep(0);
		char message[BUFFER_SIZE];

		printf("Message : ");
		fgets(message, BUFFER_SIZE, stdin);

		ST_PACKET_INFO* stPacketSend = test[i++%20];
		std::tstring jsPacketSend;
		core::WriteJsonToString(stPacketSend, jsPacketSend);

		MessageManager()->PushSendMessage(5, REQUEST, OPCODE2, "Data Return");
		MessageManager()->PushSendMessage(5, REQUEST, OPCODE3, "message1");
		MessageManager()->PushSendMessage(5, REQUEST, OPCODE2, "Data Return");
		MessageManager()->PushSendMessage(5, REQUEST, OPCODE3, "message2");
		MessageManager()->PushSendMessage(5, REQUEST, OPCODE2, "Data Return");
		MessageManager()->PushSendMessage(5, REQUEST, OPCODE3, "message3");
		MessageManager()->PushSendMessage(5, REQUEST, OPCODE2, "Data Return");
		MessageManager()->PushSendMessage(5, REQUEST, OPCODE3, "message4");
		MessageManager()->PushSendMessage(5, REQUEST, OPCODE2, "Data Return");
		/*ServerManager()->Send(6, jsPacketSend);
		ServerManager()->Send(5, jsPacketSend);*/
	}
}

int main(int argc, char* argv[])
{
	try
	{
		signal(SIGINT, interruptHandler);
		std::future<void> a = std::async(std::launch::async, &CMessage::Init, MessageManager());
		std::future<void> b = std::async(std::launch::async, &SendTest);
	}
	catch (std::exception& e)
	{
		printf("%s\n", e.what());
	}
	return 0;
}