#include "tcpconn.hpp"
#include "uv.h"

namespace antenna
{
	class _TCPConnection_UVLayer
	{
	public:
		_TCPConnection_UVLayer(
			ITCPConnectionEventHandler* event_handler,
			Queue* msgque_send,
			Queue* msgque_recv);
		~_TCPConnection_UVLayer();

	public:
		void run(
			const std::string& ipaddr,
			uint16_t port,
			uint32_t conn_id);
		void send(Message& msg);

	private:
		enum ProtoProcRet : int
		{
			PPR_ERROR = -2,
			PPR_CLOSE = -1,

			PPR_AGAIN = 0,
			PPR_PULSE,
		};

		enum ArchProtocolVersion : uint32_t
		{
			APV_Unknown,
			APV_0_1		// arch protocol 0.1
		};

		enum ArchParsingPhase : int	// arch protocol parsing phase
		{
			APP_Start = 0,
			APP_Parsing_Header_Version,
			APP_Parsing_Header_SVC_ID,
			APP_Parsing_Header_SVC_INST_ID,
			APP_Parsing_Header_Length,
			APP_Parsing_Content_MSG_ID,		// glassy to server, a part of content.
			APP_Parsing_Content
		};

		typedef struct _st_write_req : public uv_write_t
		{
			_st_write_req(std::vector<uint8_t>&& stream_data)
				: uv_write_t({ 0 })
				, buf({0})
				, data(std::move(stream_data))
			{
				buf.base = (char*)data.data();
				buf.len = (ULONG)data.size();
			}

			uv_buf_t				buf;
			std::vector<uint8_t>	data;
		} _write_req_t;

		typedef struct _st_read_obj
		{
			_st_read_obj()
				: version(APV_Unknown)
				, svc_id(0)
				, svc_inst_id(0)
				, msg_id(0)
				, content_length(0)
				, parsing_phase(APP_Start)
			{}

			void to_msg(Message& msg)
			{

				clear();
			}

			void clear()
			{
				version = std::move(VUInt(0));
				svc_id = std::move(VUInt(0));
				svc_inst_id = std::move(VUInt(0));
				content_length = std::move(VUInt(0));
				msg_id = std::move(VUInt(0));
				data = std::move(std::vector<uint8_t>());
				parsing_phase = APP_Start;
			}

			VUInt				version;
			VUInt				svc_id;
			VUInt				svc_inst_id;
			VUInt				content_length;
			VUInt				msg_id;
			std::vector<uint8_t>	data;
			ArchParsingPhase		parsing_phase;
		} _read_obj_t;

	private:
		static void		_on_connect(uv_connect_t* conn, int status);
		static void		_on_write(uv_timer_t* handle);
		static void		_on_write_end(uv_write_t* req, int status);
		static void		_on_alloc_read_buf(
								uv_handle_t* handle,
								size_t suggested_size,
								uv_buf_t* buf);
		static void		_on_read(
								uv_stream_t* stream,
								ssize_t nread,
								const uv_buf_t* buf);
		static void		_on_closed(uv_handle_t* handle);
		static void		_close(uv_stream_t* stream);
		

	private: // protocol related
		static ProtoProcRet	_proc_istrm(
								_read_obj_t& obj,
								uv_stream_t* stream,
								ssize_t nread,
								const uint8_t* buf,
								int& procbytes);
		static void			_proc_ostrm(std::vector<uint8_t>& obuffer, const Message& msg);
		static bool			_pulse(_TCPConnection_UVLayer* uvlayer,
								_read_obj_t& obj);

	private:
		uv_tcp_t					_client;
		uv_loop_t					_loop;
		uv_timer_t					_uvtm_write;
		uv_stream_t*				_stream;
		ITCPConnectionEventHandler* _evthandler;
		Queue*						_msgque_send;
		Queue*						_msgque_recv;
		uint32_t					_conn_id;
	};
}

using namespace antenna;

/* ***************************************************************************
 *                          _TCPConnection_UVLayer                           *
 * ***************************************************************************/
_TCPConnection_UVLayer::_TCPConnection_UVLayer(
	ITCPConnectionEventHandler* event_handler,
	Queue* msgque_send,
	Queue* msgque_recv)
	: _client({ 0 })
	, _loop({ 0 })
	, _uvtm_write({ 0 })
	, _stream(nullptr)
	, _evthandler(event_handler)
	, _msgque_send(msgque_send)
	, _msgque_recv(msgque_recv)
	, _conn_id(0)
{}

_TCPConnection_UVLayer::~_TCPConnection_UVLayer()
{}

void _TCPConnection_UVLayer::run(
	const std::string& ipaddr,
	uint16_t port,
	uint32_t conn_id)
{
	int uvres = 0;
	
	_conn_id = conn_id;
	uv_loop_init(&_loop);


	uvres = uv_tcp_init(&_loop, &_client);
	uvres = uv_tcp_keepalive(&_client, 1, 360);

	sockaddr_in dest;
	uvres = uv_ip4_addr(ipaddr.c_str(), port, &dest);

	uv_connect_t conn;
	conn.data = this;
	uvres = uv_tcp_connect(&conn, &_client, (const struct sockaddr*)&dest, _on_connect);

	uv_timer_init(&_loop, &_uvtm_write);
	_uvtm_write.data = this;

	uvres = uv_run(&_loop, UV_RUN_DEFAULT);
}

void _TCPConnection_UVLayer::_on_connect(uv_connect_t* conn, int status)
{
	_TCPConnection_UVLayer* uvl = (_TCPConnection_UVLayer*)conn->data;

	if (status >= 0)
	{	// connected successfully.
		uvl->_stream = conn->handle;
		uvl->_stream->data = uvl;

		uv_read_start(
			uvl->_stream,
			_on_alloc_read_buf,
			_on_read);
		uvl->_evthandler->on_event(ITCPConnectionEventHandler::Evt_Connected);
		uv_timer_start(&uvl->_uvtm_write, _on_write, 1, 0);
	}
	else
	{
		// TODO: handle these failure cases
		uvl->_evthandler->on_error(ITCPConnectionEventHandler::Err_Disconnected);
	}
}

void _TCPConnection_UVLayer::_on_write(uv_timer_t* handle)
{
	_TCPConnection_UVLayer* uvl = (_TCPConnection_UVLayer*)handle->data;
	std::vector<Message> msgs;
	if (uvl->_msgque_send->pop_batch(msgs, 100))
	{
		for (auto& msg : msgs)
		{
			std::vector<uint8_t> obuffer;
			_proc_ostrm(obuffer, msg);
			_write_req_t* req = new _write_req_t(std::move(obuffer));
			uv_write((uv_write_t*)req, (uvl->_stream), &req->buf, 1, _on_write_end);
		}
	}

	uv_timer_start(handle, _on_write, 1, 0);
}

void _TCPConnection_UVLayer::_on_write_end(uv_write_t* req, int status)
{
	_TCPConnection_UVLayer* uvl = (_TCPConnection_UVLayer*)req->handle->data;
	if (status < 0)
	{
		// TODO: handle these failure cases.
		uvl->_evthandler->on_error(ITCPConnectionEventHandler::Err_Send);
	}
	else
	{
		uvl->_evthandler->on_event(ITCPConnectionEventHandler::Evt_MsgSent);
	}
	delete (_write_req_t*)req;
}

void _TCPConnection_UVLayer::_on_read(
	uv_stream_t* stream,
	ssize_t nread,
	const uv_buf_t* buf)
{
	static _read_obj_t robj;

	_TCPConnection_UVLayer* uvl = (_TCPConnection_UVLayer*)(stream->data);
	int offset = 0;
	bool goon = nread >= 0;
	while (goon && (nread - offset) > 0)
	{
		int procbytes = 0;
		switch (_proc_istrm(
					robj,
					stream,
					nread - offset,
					(const uint8_t*)buf->base + offset,
					procbytes))
		{
		case PPR_PULSE:
			if (_pulse(uvl, robj))
			{
				uvl->_evthandler->on_event(ITCPConnectionEventHandler::Evt_MsgReceived);
			}
			else
			{
				goon = false;
				_close(stream);
				uvl->_evthandler->on_error(ITCPConnectionEventHandler::Err_IncomingData);
			}
			break;

		case PPR_AGAIN:
			break;

		case PPR_CLOSE:
		case PPR_ERROR:
			goon = false;
			robj.clear();
			_close(stream);
			break;
		}
		offset += procbytes;
	}

	if (goon)
	{
		// continue listening to next message.
		uv_read_start(stream, _on_alloc_read_buf, _on_read);
	}
	else
	{
		_close(stream);
		uvl->_evthandler->on_error(ITCPConnectionEventHandler::Err_Receive);
	}

	delete buf->base;
}

_TCPConnection_UVLayer::ProtoProcRet _TCPConnection_UVLayer::_proc_istrm(
	_read_obj_t& obj,
	uv_stream_t* stream,
	ssize_t nread,
	const uint8_t* readbuf,
	int& procbytes)
{
	procbytes = 0;

	bool goon = true;
	VUInt::DigestStatus digst;
	while (goon && (nread - procbytes) > 0)
	{
		switch (obj.parsing_phase)
		{
		case APP_Start:
			obj.parsing_phase = APP_Parsing_Header_Version;
			// continue to the next phase, no BREAK here!

		case APP_Parsing_Header_Version:
			digst = obj.version.digest(readbuf[procbytes++]);
			if (VUInt::DS_Idle == digst)
			{
				if (obj.version.value() != APV_0_1)
				{
					return PPR_ERROR;
				}
				obj.parsing_phase = APP_Parsing_Header_SVC_ID;
			}
			else if(VUInt::DS_Bad == digst)
			{
				return PPR_ERROR;
			}
			break;

		case APP_Parsing_Header_SVC_ID:
			digst = obj.svc_id.digest(readbuf[procbytes++]);
			if (VUInt::DS_Idle == digst)
			{
				obj.parsing_phase = APP_Parsing_Header_SVC_INST_ID;
			}
			else if (VUInt::DS_Bad == digst)
			{
				return PPR_ERROR;
			}
			break;

		case APP_Parsing_Header_SVC_INST_ID:
			digst = obj.svc_inst_id.digest(readbuf[procbytes++]);
			if (VUInt::DS_Idle == digst)
			{
				if (obj.content_length.value() > 65536)
				{ // max content leng is 65536 bytes
					return PPR_ERROR;
				}
				obj.parsing_phase = APP_Parsing_Header_Length;
			}
			else if(VUInt::DS_Bad == digst)
			{
				return PPR_ERROR;
			}
			break;

		case APP_Parsing_Header_Length:
			digst = obj.content_length.digest(readbuf[procbytes++]);
			if (VUInt::DS_Idle == digst)
			{
				obj.parsing_phase = APP_Parsing_Content_MSG_ID;
			}
			else if (VUInt::DS_Bad == digst)
			{
				return PPR_ERROR;
			}

		case APP_Parsing_Content_MSG_ID:
			digst = obj.msg_id.digest(readbuf[procbytes++]);
			if (VUInt::DS_Idle == digst)
			{
				obj.content_length.value(
					obj.content_length.value() -
					obj.msg_id.encoded_size());
				obj.parsing_phase = APP_Parsing_Content;
			}
			else
			{
				return PPR_ERROR;
			}
			break;

		case APP_Parsing_Content:
		{
			uint32_t toreadcnt =
				obj.content_length.value() -
				(uint32_t)obj.data.size();
			if (toreadcnt > 0)
			{
				toreadcnt = ((uint32_t)nread - procbytes) < toreadcnt ?
					((uint32_t)nread - procbytes) : toreadcnt;
				obj.data.insert(
					obj.data.end(),
					readbuf + procbytes,
					readbuf + procbytes + toreadcnt);
				procbytes += toreadcnt;

				if ((uint32_t)obj.data.size() == obj.content_length.value())
				{
					return PPR_PULSE;
				}
			}
			else
			{
				return PPR_ERROR;
			}
		}
			break;
			
		default:
			return PPR_ERROR;
		}
	}

	return PPR_AGAIN;
}

void _TCPConnection_UVLayer::_proc_ostrm(
	std::vector<uint8_t>& obuffer,
	const Message& msg)
{
	static VUInt version(APV_0_1);

	// pack version
	obuffer << version;

	// pack svc id
	obuffer << msg.get_service_id();

	// pack svc inst id
	obuffer << msg.get_service_instance_id();

	// pack content length
	VUInt clen(msg.get_message_id().encoded_size() + msg.get_length());
	obuffer << clen;

	// pack content
	obuffer << msg.get_message_id();	// to server, msg id is a part of content that is glassy.
	obuffer.insert(
		obuffer.end(),
		msg.get_data().cbegin(),
		msg.get_data().cend());	
}

bool _TCPConnection_UVLayer::_pulse(
	_TCPConnection_UVLayer* uvlayer,
	_read_obj_t& obj)
{
	Message msg;
	msg.set_connection_id(uvlayer->_conn_id);
	msg.set_service_id(std::move(obj.svc_id));
	msg.set_service_instance_id(std::move(obj.svc_inst_id));
	msg.set_message_id(std::move(obj.msg_id));
	msg.set_data(std::move(obj.data));
	obj.clear();

	uvlayer->_msgque_recv->push(std::move(msg));

	return true;
}

void _TCPConnection_UVLayer::_on_alloc_read_buf(
	uv_handle_t* handle,
	size_t suggested_size,
	uv_buf_t* buf)
{
	buf->base = new char[suggested_size];
	buf->len = (ULONG)suggested_size;
}

void _TCPConnection_UVLayer::_close(uv_stream_t* stream)
{
	if (!uv_is_closing((uv_handle_t*)stream))
	{
		uv_close((uv_handle_t*)stream, _on_closed);
	}
}

void _TCPConnection_UVLayer::_on_closed(uv_handle_t* handle)
{}

void _TCPConnection_UVLayer::send(Message& msg)
{
	_msgque_send->push(std::move(msg));
}

/* ***************************************************************************
 *                               TCPConnection                               *
 * ***************************************************************************/
TCPConnection::TCPConnection(
	ITCPConnectionEventHandler* handler,
	Queue* msgque_send,
	Queue* msgque_recv)
	: _thrd(nullptr)
	, _port(0)
	, _conn_id(0)
	, _uvlayer(
		std::make_unique<_TCPConnection_UVLayer>(
			handler,
			msgque_send,
			msgque_recv))
	, _event_handler(handler)
{}

TCPConnection::~TCPConnection()
{}

void TCPConnection::connect(
	const std::string& ipaddr,
	uint16_t port,
	uint32_t conn_id)
{
	assert(_thrd == nullptr);
	_ipaddr = ipaddr;
	_port = port;
	_conn_id = conn_id;
	_thrd = new std::thread(std::bind(&TCPConnection::_thread, this));
}

void TCPConnection::disconnect()
{}

void TCPConnection::send(Message& message)
{
	_uvlayer->send(message);
}

void TCPConnection::_thread()
{
	_uvlayer->run(_ipaddr, _port, _conn_id);
	std::cout << "exit thread: TCPConnection" << std::endl;
}


