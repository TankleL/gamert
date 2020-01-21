#pragma once

#include "pre-req.hpp"

namespace ConfigNetworks
{
	typedef struct _st_connection
	{
		std::string		remote_ipaddr;
		std::uint16_t	port;
		int				server_type;
		bool			enable_heartbeats;
	} connection_t;

	extern std::unordered_map<std::string, std::any>	constants;
	extern std::vector<connection_t>					connections;
}
