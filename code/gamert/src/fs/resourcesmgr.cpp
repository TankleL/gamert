#include "resourcesmgr.hpp"

ResourcesMgr& ResourcesMgr::get_instance()
{
	static ResourcesMgr inst;
	return inst;
}

ResourcesMgr::ResourcesMgr()
	: _res_root(TEST_RESOURCES_ROOT)
{}

void ResourcesMgr::set_resources_root_path(const std::string& path)
{
	_res_root = path;
}

