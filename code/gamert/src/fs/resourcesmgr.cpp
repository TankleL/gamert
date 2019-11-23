#include "resourcesmgr.hpp"
#include "fileop.hpp"

ResourcesMgr::ResourcesMgr()
	: _res_root(TEST_RESOURCES_ROOT)
{}

void ResourcesMgr::set_resources_root_path(const std::string& path)
{
	_res_root = path;
}

void ResourcesMgr::read_binary_file(std::vector<std::uint8_t>& dest, const std::string& respath) const
{
	fileop::read_binary_file(dest, fullpath(respath));
}

std::string ResourcesMgr::fullpath(const std::string& respath) const
{
	return _res_root + "/" + respath;
}

