#include "networksmgr.hpp"
#include "config-networks.hpp"

NetworksMgr::NetworksMgr()
{}


void NetworksMgr::startup()
{
	_reload_networkcfg();
}

void NetworksMgr::shutdown()
{
}

void NetworksMgr::reconnect(bool force)
{
	
}

void NetworksMgr::_reload_networkcfg()
{
	//antenna::config::tcp_connections.clear();
	//for (const auto& cfg : ConfigNetworks::connections)
	//{
	//	antenna::config::tcp_connections.push_back(
	//		antenna::config::tcp_connection_t(
	//			cfg.remote_ipaddr,
	//			cfg.port,
	//			cfg.connection_id
	//		));
	//}
}

