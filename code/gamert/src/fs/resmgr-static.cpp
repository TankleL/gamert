#include "resmgr-static.hpp"
#include "fileop.hpp"

ResMgrStatic::ResMgrStatic()
	: _res_root(TEST_RESOURCES_ROOT)
{}

void ResMgrStatic::set_resources_root_path(const std::string& path)
{
	_res_root = path;
}

void ResMgrStatic::read_binary_file(std::vector<std::uint8_t>& dest, const std::string& respath) const
{
	fileop::read_binary_file(dest, fullpath(respath));
}

std::string ResMgrStatic::fullpath(const std::string& respath) const
{
	return _res_root + "/" + respath;
}

