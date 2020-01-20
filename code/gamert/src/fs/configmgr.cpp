#include "configmgr.hpp"
#include "tinyxml2.h"
#include "resmgr-static.hpp"

using namespace std;

const char* HardCoded_DefaultMasterConfigPath = "configs/master.xml";

ConfigMgr::ConfigMgr()
{}

void ConfigMgr::load_config()
{
	_load_cfg_master();

	const auto& configpath = ConfigMaster::subconfigs.find("Networks");
	GRT_CHECK(configpath != ConfigMaster::subconfigs.end(), "networks config not found")
	_load_cfg_networks(configpath->second);

}

void ConfigMgr::_load_cfg_master()
{
	try
	{
		tinyxml2::XMLDocument xdoc;
		xdoc.LoadFile(
			ResMgrStatic::get_instance()
			.fullpath(HardCoded_DefaultMasterConfigPath)
			.c_str());

		const auto& xgraph = xdoc.RootElement();

		if (GRT_IS_STRING_EQUAL(
			xgraph->Name(),
			"MasterConfig"))
		{
			_load_cfg_master_subcfgs(xgraph->LastChildElement("SubConfigs"));
		}
		else
		{
			throw std::runtime_error(
				"unknown file format.");
		}
	}
	catch (std::exception ex)
	{
		// TODO: handle exceptions
		throw ex;
	}
}

void ConfigMgr::_load_cfg_master_subcfgs(const void* xnode)
{
	const tinyxml2::XMLElement& xsubcfgs = *((const tinyxml2::XMLElement*)xnode);
	const tinyxml2::XMLElement* xscfg = xsubcfgs.FirstChildElement();

	ConfigMaster::subconfigs.clear();

	while (nullptr != xscfg)
	{
		if (GRT_IS_STRING_EQUAL(
			xscfg->Name(),
			"Config"))
		{
			const char* cname = xscfg->Attribute("Name");
			const char* cfile = xscfg->Attribute("File");
			
			ConfigMaster::subconfigs[cname] = cfile;
		}
		xscfg = xscfg->NextSiblingElement();
	}
}

void ConfigMgr::_load_cfg_networks(const std::string& configpath)
{
	
}
