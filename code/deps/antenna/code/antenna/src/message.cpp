#include "message.hpp"

using namespace std;
using namespace antenna;

Message::Message()
{}

Message::Message(const Message& rhs)
	: _svc_id(rhs._svc_id)
	, _svc_inst_id(rhs._svc_inst_id)
	, _conn_id(rhs._conn_id)
	, _msg_id(rhs._msg_id)
	, _data(rhs._data)
{}

Message::Message(Message&& rhs) noexcept
	: _svc_id(std::move(rhs._svc_id))
	, _svc_inst_id(std::move(rhs._svc_inst_id))
	, _conn_id(std::move(rhs._conn_id))
	, _msg_id(std::move(rhs._msg_id))
	, _data(std::move(rhs._data))
{}

Message::~Message()
{}

Message& Message::operator=(const Message& rhs)
{
	_svc_id = rhs._svc_id;
	_svc_inst_id = rhs._svc_inst_id;
	_conn_id = rhs._conn_id;
	_msg_id = rhs._msg_id;
	_data = rhs._data;

	return *this;
}

Message& Message::operator=(Message&& rhs) noexcept
{
	_svc_id = std::move(rhs._svc_id);
	_svc_inst_id = std::move(rhs._svc_inst_id);
	_conn_id = std::move(rhs._conn_id);
	_msg_id = std::move(rhs._msg_id);
	_data = std::move(rhs._data);
	return *this;
}

const std::vector<uint8_t>& Message::get_data() const
{
	return _data;
}

std::vector<uint8_t>&& Message::acquire_data()
{
	return std::move(_data);
}

void Message::set_data(
	const std::vector<uint8_t>& data,
	uint16_t len,
	uint16_t offset)
{
	_data.clear();
	_data.insert(
		_data.end(),
		data.begin() + offset,
		data.begin() + offset + len
	);
}

void Message::set_data(std::vector<uint8_t>&& data)
{
	_data = std::move(data);
}

void Message::set_data(const std::vector<uint8_t>& data)
{
	_data = data;
}

void Message::append_data(
	const std::vector<uint8_t>& data,
	uint16_t len,
	uint16_t offset)
{
	_data.insert(
		_data.end(),
		data.begin() + offset,
		data.begin() + offset + len
	);
}

uint16_t Message::get_length() const
{
	assert(_data.size() < 65536);
	return (uint16_t)_data.size();
}

void Message::set_service_id(uint32_t id)
{
	_svc_id.value(id);
}

void Message::set_service_id(VUInt&& id)
{
	_svc_id = std::move(id);
}

void Message::set_service_instance_id(uint16_t id)
{
	_svc_inst_id.value(id);
}

void Message::set_service_instance_id(VUInt&& id)
{
	_svc_inst_id = std::move(id);
}

void Message::set_message_id(uint32_t id)
{
	_msg_id.value(id);
}

void Message::set_message_id(VUInt&& id)
{
	_msg_id = std::move(id);
}

void Message::set_connection_id(uint32_t id)
{
	_conn_id.value(id);
}

void Message::set_connection_id(VUInt&& id)
{
	_conn_id = std::move(id);
}

const VUInt& Message::get_service_id() const
{
	return _svc_id;
}

const VUInt& Message::get_service_instance_id() const
{
	return _svc_inst_id;
}

const VUInt& Message::get_message_id() const
{
	return _msg_id;
}

const VUInt& Message::get_connection_id() const
{
	return _conn_id;
}




