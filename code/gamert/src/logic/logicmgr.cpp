#include "logicmgr.hpp"

LogicMgr::LogicMgr()
	: _tick(0)
{
	_timer.snapshot();
}

void LogicMgr::tick()
{
	float elapsed = _timer.elapsed();
	_timer.snapshot();

	LNode::tick_param_t tick_param;
	tick_param.elapsed = elapsed;
	tick_param.tick = _tick;

	_root.tick(tick_param);

	++_tick;
}

LNode& LogicMgr::root()
{
	return _root;
}

void LogicMgr::register_lnode_creator(
	const std::string& name,
	const creator_t& creator)
{
	auto& node = _lnode_creator.find(name);

	GRT_CHECK(
		node == _lnode_creator.end(),
		"a creator with the same name already exists.");

	_lnode_creator[name] = creator;
}

void LogicMgr::unregister_lnode_creators()
{
	_lnode_creator.clear();
}

LNode* LogicMgr::create_lnode(const std::string& name)
{
	auto& creator = _lnode_creator.find(name);

	GRT_CHECK(
		creator != _lnode_creator.end(),
		"node creator not found.");

	return creator->second();
}



