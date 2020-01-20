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
	// parse and load master config
	void _load_cfg_master();
	void _load_cfg_master_subcfgs(const void* xnode);

	// parse and load networks config
	void _load_cfg_networks(const std::string& configpath);

};

