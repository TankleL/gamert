#include "networksmgr.hpp"

NetworksMgr::NetworksMgr()
{}


void NetworksMgr::startup()
{
	_antenna.startup();
}

void NetworksMgr::shutdown()
{
	_antenna.shutdown();
}

void NetworksMgr::reconnect(bool force)
{
	
}
