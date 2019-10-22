#pragma once

#include "pre-req.hpp"

class ResourcesMgr
{
public:
	static ResourcesMgr& get_instance();

public:
	void set_resources_root_path(const std::string& path);

private:
	ResourcesMgr();

private:
	std::string	_res_root;
};

