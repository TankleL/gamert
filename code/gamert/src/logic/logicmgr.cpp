#include "logicmgr.hpp"

LogicMgr::LogicMgr()
{}

LogicMgr& LogicMgr::get_instance()
{
	static LogicMgr lmgr;
	return lmgr;
}

void LogicMgr::update_frame()
{

}

