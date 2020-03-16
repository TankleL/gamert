#pragma once
#include "antenna-prereq.hpp"
#include "queue.hpp"

namespace antenna
{
	class _TCPConnection_UVLayer;
	class ITCPConnectionEventHandler;
	class TCPConnection
	{
	public:
		TCPConnection(
			ITCPConnectionEventHandler* handler,
			Queue* msgque_send,
			Queue* msgque_recv);
		virtual ~TCPConnection();

	public:
		void connect(
			const std::string& ipaddr,
			uint16_t port,
			uint32_t conn_id);
		void disconnect();
		void send(Message& message);

	private:
		void _thread();

	private:
		std::string		_ipaddr;
		uint16_t		_port;
		uint32_t		_conn_id;
		std::thread*	_thrd;

	private:
		ITCPConnectionEventHandler*				_event_handler;
		std::unique_ptr<_TCPConnection_UVLayer>	_uvlayer;
	};

	class ITCPConnectionEventHandler
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
		virtual void on_error(Errors err) = 0;
		virtual void on_event(Events evt) = 0;
	};
}

