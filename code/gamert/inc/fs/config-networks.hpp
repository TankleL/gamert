#pragma once

#include "pre-req.hpp"

namespace ConfigNetworks
{
	typedef struct _st_connection
	{
		std::string server_type;
		std::string remote_ipaddr;
		int			port;
		bool		enable_heartbeats;
	} connection_t;

	extern std::vector<connection_t>	connections;
}
