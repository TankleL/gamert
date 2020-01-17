#pragma once

#include "pre-req.hpp"
#include "singleton.hpp"
#include "config-master.hpp"
#include "config-networks.hpp"

class ConfigMgr : public Singleton<ConfigMgr>
{
	DECL_SINGLETON_CTOR(ConfigMgr);
public:
	void load_config();

private:
	void _load_cfg_master();
	void _load_cfg_network();

};

