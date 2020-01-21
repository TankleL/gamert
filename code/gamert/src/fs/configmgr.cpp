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

		const auto& xmastercfg = xdoc.RootElement();

		if (GRT_IS_STRING_EQUAL(
			xmastercfg->Name(),
			"MasterConfig"))
		{
			_load_cfg_master_subcfgs(xmastercfg->LastChildElement("SubConfigs"));
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
	try
	{
		tinyxml2::XMLDocument xdoc;
		xdoc.LoadFile(
			ResMgrStatic::get_instance()
			.fullpath(configpath)
			.c_str());

		const auto& xnetworkscfg = xdoc.RootElement();

		if (GRT_IS_STRING_EQUAL(
			xnetworkscfg->Name(),
			"NetworksConfig"))
		{
			_load_cfg_networks_constants(xnetworkscfg);
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

void ConfigMgr::_load_cfg_networks_constants(const void* xnetworkscfg)
{
	const tinyxml2::XMLElement& xntcfg = *((const tinyxml2::XMLElement*)xnetworkscfg);
	const  tinyxml2::XMLElement* xiter = xntcfg.FirstChildElement();
	while (nullptr != xiter)
	{
		if (GRT_IS_STRING_EQUAL(
			xiter->Name,
			"Const"))
		{
			if (GRT_IS_STRING_EQUAL(
				xiter->Attribute("Type"),
				"int32"))
			{
				ConfigNetworks::constants[xiter->Attribute("Name")] =
					xiter->IntAttribute("Value");
			}
		}
		xiter = xiter->NextSiblingElement();
	}
}


void ConfigMgr::_load_cfg_networks_connections(const void* xnetworkscfg)
{
	const tinyxml2::XMLElement& xntcfg = *((const tinyxml2::XMLElement*)xnetworkscfg);
	const  tinyxml2::XMLElement* xiter = xntcfg.FirstChildElement();
	ConfigNetworks::connections.clear();
	while (nullptr != xiter)
	{
		if (GRT_IS_STRING_EQUAL(
			xiter->Name,
			"Connection"))
		{
			ConfigNetworks::connection_t	conn;
			conn.remote_ipaddr = xiter->Attribute("RemoteAddr");
			conn.port = (uint16_t)xiter->IntAttribute("Port");
			conn.enable_heartbeats = xiter->BoolAttribute("EnableHeartbeats");

			ConfigNetworks::connections.push_back(conn);
		}
	}
}
