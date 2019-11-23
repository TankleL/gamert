#include "logicmgr.hpp"

LNode LogicMgr::_dummy_node;

LogicMgr::LogicMgr()
	: _tick(0)
	, _root_node(&_dummy_node)
{
	_dummy_node.set_name("dummy");
	_timer.snapshot();
}

void LogicMgr::tick()
{
	float elapsed = _timer.elapsed();
	_timer.snapshot();

	LNode::tick_param_t tick_param;
	tick_param.elapsed = elapsed;
	tick_param.tick = _tick;

	_root_node->tick(tick_param);

	++_tick;
}

LNode* LogicMgr::get_root_node() const
{
	return _root_node;
}

LNode* LogicMgr::switch_root_node(LNode* new_node)
{
	if (nullptr == new_node)
		new_node = &_dummy_node;

	LNode * old_node = _root_node;
	_root_node = new_node;

	return old_node;
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



