#include "antenna.hpp"
#include <cstdlib>
#include <cstdio>
#include <csignal>

using namespace antenna;


Antenna global_at;

void test_sendmsg(Antenna& at)
{
	char test[] = "Hello Arch Server!";
	std::vector<uint8_t> greeting;
	greeting.insert(greeting.end(), test, test + sizeof(test));
	
	for(int i = 0; i < 100000; ++ i)
	{
		Message msg;
		
		msg.set_service_id(8080);
		msg.set_message_id(2);
		msg.set_connection_id(1);
		msg.set_data(greeting);

		at.send_message(msg);
	}
}

void test_getmsg(Antenna& at)
{
	static int n = 0;
	std::vector<Message> msglist;
	at.get_messages(msglist);
	n += msglist.size();

	if (n % 1000 == 0)
	{
		std::cout << "recv: " << n << std::endl;
	}
}

class AtHandler : public IAntennaEventHandler
{
public:
	virtual void on_error(Antenna& at, Errors err) override
	{
		std::cout << "antenna error" << std::endl;
	}

	virtual void on_event(Antenna& at, Events evt) override
	{
		switch (evt)
		{
		case antenna::IAntennaEventHandler::Evt_Connected:
			std::cout << "Connected" << std::endl;
			test_sendmsg(at);
			break;

		case antenna::IAntennaEventHandler::Evt_MsgSent:
			break;

		case antenna::IAntennaEventHandler::Evt_MsgReceived:
			test_getmsg(at);
			break;

		default:
			break;
		}
	}
};

void on_signal(int sig)
{
	global_at.shutdown();
	exit(0);
}

int main(int argc, char** argv)
{
	antenna::config::tcp_connections.push_back(
		config::tcp_connection_t(
			"127.0.0.1",
			8088,
			1
		));

	global_at.startup(std::make_shared<AtHandler>());

	signal(SIGINT, on_signal);
	system("pause");
	global_at.shutdown();

	return 0;
}