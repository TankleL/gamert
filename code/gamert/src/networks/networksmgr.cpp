#include "networksmgr.hpp"
#include "config-networks.hpp"

NetworksMgr::NetworksMgr()
	: _athdlr(std::make_shared<_AtEvtHandler>())
{}


void NetworksMgr::startup()
{
	_reload_networkcfg();
	_antenna.startup(_athdlr);
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
	for (const auto& cfg : ConfigNetworks::connections)
	{
		antenna::config::tcp_connections.push_back(
			antenna::config::tcp_connection_t(
				cfg.remote_ipaddr,
				cfg.port,
				cfg.connection_id
			));
	}
}

void NetworksMgr::_AtEvtHandler::on_event(
	antenna::Antenna& antenna,
	Events evt)
{

}

void NetworksMgr::_AtEvtHandler::on_error(
	antenna::Antenna& antenna,
	Errors err)
{

}
