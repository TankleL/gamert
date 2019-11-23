#pragma once

#include "pre-req.hpp"
#include "singleton.hpp"

class ResourcesMgr : public Singleton<ResourcesMgr>
{
	DECL_SINGLETON_CTOR(ResourcesMgr);

public:
	void set_resources_root_path(const std::string& path);
	void read_binary_file(std::vector<std::uint8_t>& dest, const std::string& respath) const;
	std::string fullpath(const std::string& respath) const;

private:
	std::string	_res_root;
};

