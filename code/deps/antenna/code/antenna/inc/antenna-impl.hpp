#pragma once
#include "antenna-prereq.hpp"
#include "message.hpp"
#include "queue.hpp"
#include "tcpconn.hpp"

namespace antenna
{
	class Antenna;
	class IAntennaEventHandler
	{
	public:
		enum Events : int32_t
		{
			Evt_Connected,
			Evt_MsgSent,
			Evt_MsgReceived,
		};

		enum Errors : int32_t
		{
			Err_Disconnected,
			Err_Send,
			Err_Receive,
			Err_IncomingData,
		};

	public:
		virtual void on_event(Antenna& antenna, Events evt) = 0;
		virtual void on_error(Antenna& antenna, Errors err) = 0;
	};

	class Antenna : public ITCPConnectionEventHandler
	{
	public:
		Antenna();
		virtual ~Antenna();

	public:
		void startup(const std::shared_ptr<IAntennaEventHandler>& handler);
		void shutdown();

		void send_message(Message& msg);
		void get_messages(std::vector<Message>& msglist);

	public:
		virtual void on_error(ITCPConnectionEventHandler::Errors err) override;
		virtual void on_event(ITCPConnectionEventHandler::Events evt) override;

	private:
		typedef std::unordered_map<uint32_t, std::shared_ptr<TCPConnection>> tcpconns_t;

	private:
		tcpconns_t		_tcpconns;
		Queue			_msgque_send;
		Queue			_msgque_recv;

	private:
		static std::unordered_map<
			ITCPConnectionEventHandler::Events,
			IAntennaEventHandler::Events> _tcp_events;
		static std::unordered_map<
			ITCPConnectionEventHandler::Errors,
			IAntennaEventHandler::Errors> _tcp_errors;

	private:
		std::shared_ptr<IAntennaEventHandler>	_event_handler;
	};
}
