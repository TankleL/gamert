#include "logicmgr.hpp"

LSceneGraph LogicMgr::_dummy_scene;

LogicMgr::LogicMgr()
	: _tick(0)
	, _scene(&_dummy_scene)
{
	_scene->set_name("dummy");
	_timer.snapshot();
}

void LogicMgr::tick()
{
	float elapsed = _timer.elapsed();
	_timer.snapshot();

	LNode::tick_param_t tick_param;
	tick_param.elapsed = elapsed;
	tick_param.tick = _tick;

	_scene->tick(tick_param);

	++_tick;
}

LSceneGraph* LogicMgr::get_scene_graph() const
{
	return _scene;
}

LSceneGraph* LogicMgr::switch_scene_graph(LSceneGraph* new_scene)
{
	if (nullptr == new_scene)
		new_scene = &_dummy_scene;

	LSceneGraph* old = _scene;
	_scene = new_scene;

	return old;
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



