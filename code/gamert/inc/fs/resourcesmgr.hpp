#pragma once

#include "pre-req.hpp"

class ResourcesMgr
{
public:
	static ResourcesMgr& get_instance();

public:
	void set_resources_root_path(const std::string& path);

	void read_binary_file(std::vector<std::uint8_t>& dest, const std::string& respath) const;

private:
	ResourcesMgr();

private:
	std::string	_res_root;
};

