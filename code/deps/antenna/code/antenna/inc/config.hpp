#pragma once

#include "antenna-prereq.hpp"

namespace antenna
{
	namespace config
	{
		typedef struct _st_tcp_connection
		{
			_st_tcp_connection()
				: port(0)
				, conn_id(0)
			{}

			_st_tcp_connection(
				const std::string& conn_ipaddr,
				uint16_t conn_port,
				int connection_id)
				: ipaddr(conn_ipaddr)
				, port(conn_port)
				, conn_id(connection_id)
			{}

			std::string		ipaddr;
			uint16_t		port;
			uint32_t		conn_id;
		} tcp_connection_t;


		extern std::vector<tcp_connection_t>	tcp_connections;
	}
}


