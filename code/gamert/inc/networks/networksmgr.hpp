#pragma once

#include "pre-req.hpp"
#include "singleton.hpp"
#include "antenna.hpp"

class NetworksMgr
	: public Singleton<NetworksMgr>
	, public antenna::IAntennaEventHandler
{
	DECL_SINGLETON_CTOR(NetworksMgr);
public:
	void startup();
	void shutdown();

	void reconnect(bool force);

public:
	// Antenna Event Handling
	virtual void on_connected(antenna::Antenna& antenna) override;
	virtual void on_connection_error(antenna::Antenna& antenna) override;

private:
	void _reload_networkcfg();

private:
	antenna::Antenna	_antenna;
};


