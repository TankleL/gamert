#include "antenna-impl.hpp"
#include "tcpconn.hpp"
#include "config.hpp"

using namespace antenna;


std::unordered_map<
	ITCPConnectionEventHandler::Events,
	IAntennaEventHandler::Events> antenna::Antenna::_tcp_events({
		{ITCPConnectionEventHandler::Evt_Connected, IAntennaEventHandler::Evt_Connected},
		{ITCPConnectionEventHandler::Evt_MsgSent, IAntennaEventHandler::Evt_MsgSent},
		{ITCPConnectionEventHandler::Evt_MsgReceived, IAntennaEventHandler::Evt_MsgReceived},
	});

std::unordered_map<
	ITCPConnectionEventHandler::Errors,
	IAntennaEventHandler::Errors> antenna::Antenna::_tcp_errors({
		{ITCPConnectionEventHandler::Err_Disconnected, IAntennaEventHandler::Err_Disconnected},
		{ITCPConnectionEventHandler::Err_Send, IAntennaEventHandler::Err_Send},
		{ITCPConnectionEventHandler::Err_Receive, IAntennaEventHandler::Err_Receive},
		{ITCPConnectionEventHandler::Err_IncomingData, IAntennaEventHandler::Err_IncomingData},
	});

Antenna::Antenna()
{}

Antenna::~Antenna()
{}

void Antenna::startup(const std::shared_ptr<IAntennaEventHandler>& handler)
{
	_event_handler = handler;
	for (const auto& tcpcfg : config::tcp_connections)
	{
		std::shared_ptr<TCPConnection> conn =
			std::make_shared<TCPConnection>(
				this,
				&_msgque_send,
				&_msgque_recv);

		_tcpconns[tcpcfg.conn_id] = conn;
		_tcpconns[tcpcfg.conn_id]->connect(tcpcfg.ipaddr, tcpcfg.port, tcpcfg.conn_id);
	}
}

void Antenna::shutdown()
{
}

void Antenna::send_message(Message& msg)
{
	auto conn = _tcpconns.find(msg.get_connection_id().value());
	if (conn != _tcpconns.end())
	{
		conn->second->send(msg);
	}
	else
	{
		throw std::runtime_error("Connection not found.");
	}
}

void Antenna::get_messages(std::vector<Message>& msglist)
{
	_msgque_recv.pop_all(msglist);
}

void Antenna::on_error(ITCPConnectionEventHandler::Errors err)
{
	_event_handler->on_error(*this, _tcp_errors[err]);
}

void Antenna::on_event(ITCPConnectionEventHandler::Events evt)
{
	_event_handler->on_event(*this, _tcp_events[evt]);
}

