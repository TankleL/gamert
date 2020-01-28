#include "networksmgr.hpp"
#include "config-networks.hpp"

NetworksMgr::NetworksMgr()
{}


void NetworksMgr::startup()
{
	_reload_networkcfg();
	_antenna.startup(this);
}

void NetworksMgr::shutdown()
{
	_antenna.shutdown();
}

void NetworksMgr::reconnect(bool force)
{
	
}

void NetworksMgr::_reload_networkcfg()
{
	antenna::config::tcp_connections.clear();
	for (const auto& cfg : antenna::config::tcp_connections)
	{
		antenna::config::tcp_connections.push_back(
			antenna::config::tcp_connection_t(
				cfg.ipaddr,
				cfg.port,
				cfg.server_type
			));
	}
}

void NetworksMgr::on_connected(antenna::Antenna& antenna)
{

}

void NetworksMgr::on_connection_error(antenna::Antenna& antenna)
{

}
