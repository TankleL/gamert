#pragma once
#include "pre-req.hpp"
#include "singleton.hpp"
#include "lotus.hpp"

class NetworksMgr
	: public Singleton<NetworksMgr>
{
	DECL_SINGLETON_CTOR(NetworksMgr);
public:
	void startup();
	void shutdown();

	void reconnect(bool force = false);

private:
private:
	void _reload_networkcfg();

private:
};


