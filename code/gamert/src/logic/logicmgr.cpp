#include "logicmgr.hpp"

LogicMgr::LogicMgr()
	: _tick(0)
{}

void LogicMgr::tick()
{
	_root.tick();
}

LNode& LogicMgr::root()
{
	return _root;
}
