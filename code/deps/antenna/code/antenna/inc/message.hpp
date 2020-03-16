#pragma once
#include "antenna-prereq.hpp"
#include "vuint.hpp"

namespace antenna
{
	class Message
	{
	public:
		Message();
		Message(const Message& rhs);
		Message(Message&& rhs) noexcept;
		virtual ~Message();

	public:
		Message& operator=(const Message& rhs);	
		Message& operator=(Message&& rhs) noexcept;

	public:
		void			set_service_id(uint32_t id);
		void			set_service_id(VUInt&& id);
		void			set_service_instance_id(uint16_t id);
		void			set_service_instance_id(VUInt&& id);
		void			set_message_id(uint32_t id);
		void			set_message_id(VUInt&& id);
		void			set_connection_id(uint32_t id);
		void			set_connection_id(VUInt&& id);
		void			set_data(
							const std::vector<uint8_t>& data,
							uint16_t len,
							uint16_t offset = 0);
		void			set_data(std::vector<uint8_t>&& data);
		void			set_data(const std::vector<uint8_t>& data);
		void			append_data(
							const std::vector<uint8_t>& data,
							uint16_t len,
							uint16_t offset = 0);

		const VUInt&	get_service_id() const;
		const VUInt&	get_service_instance_id() const;
		const VUInt&	get_message_id() const;
		const VUInt&	get_connection_id() const;
		const std::vector<uint8_t>&	get_data() const;
		std::vector<uint8_t>&& acquire_data();
		uint16_t		get_length() const;

	private:
		VUInt					_svc_id;
		VUInt					_svc_inst_id;
		VUInt					_conn_id;
		VUInt					_msg_id;
		std::vector<uint8_t>	_data;
	};
}

