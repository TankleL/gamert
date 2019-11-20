#pragma once

#include "pre-req.hpp"
#include "singleton.hpp"
#include "lnode.hpp"

class LogicMgr : public Singleton<LogicMgr>
{
	DECL_SINGLETON_CTOR(LogicMgr);

public:
	void tick();
	LNode& root();

private:
	LNode		_root;
	uint32_t	_tick;
};



