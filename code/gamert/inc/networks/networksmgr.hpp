#pragma once

#include "pre-req.hpp"
#include "singleton.hpp"
#include "antenna.hpp"

class NetworksMgr
	: public Singleton<NetworksMgr>
{
	DECL_SINGLETON_CTOR(NetworksMgr);
public:
	void startup();
	void shutdown();

	void reconnect(bool force = false);

private:
	class _AtEvtHandler : public antenna::IAntennaEventHandler
	{
	public:
		// Antenna Event Handling
		virtual void on_event(antenna::Antenna& antenna, Events evt) override;
		virtual void on_error(antenna::Antenna& antenna, Errors err) override;
	};

private:
	void _reload_networkcfg();

private:
	antenna::Antenna				_antenna;
	std::shared_ptr<_AtEvtHandler> _athdlr;
};


