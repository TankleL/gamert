#pragma once

#include "pre-req.hpp"
#include "singleton.hpp"

class LogicMgr : public Singleton<LogicMgr>
{
	DECL_SINGLETON_CTOR(LogicMgr);

public:
	void update_frame();
};

