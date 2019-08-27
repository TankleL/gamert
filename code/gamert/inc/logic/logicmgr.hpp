#pragma once

#include "pre-req.hpp"

class LogicMgr
{
private:
	LogicMgr();

public:
	static LogicMgr& get_instance();

public:
	void update_frame();
};

