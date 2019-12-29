#include "logicmgr.hpp"

LogicMgr::LogicMgr()
{
	_timer.snapshot();
}

void LogicMgr::tick()
{
	//_scene->tick(tick_param);
	for (auto& chnl : _channels)
	{
		if (chnl.elapsed >= chnl.tick_itvl)
		{
			LNode::tick_param_t tick_param;
			tick_param.elapsed = chnl.elapsed;
			tick_param.ticks = chnl.ticks;

			// this process may cost more time than expected to run.
			chnl.scene->tick(tick_param);

			chnl.elapsed = 0.f;
			chnl.ticks += 1;
		}
		else
		{
			chnl.elapsed += _timer.elapsed();
		}
	}

	_timer.snapshot();
}

void LogicMgr::add_channel(
	const std::string& unique_name,
	float tick_interval,
	LSceneGraph* scene)
{
	if (!get_channel(unique_name))
	{
		_channel_internal_t chnl;
		chnl.name = unique_name;
		chnl.tick_itvl = tick_interval;
		chnl.scene = scene;
		_channels.push_back(chnl);
	}
	else
	{
		throw std::runtime_error(
			"a channel with the same has been already existing.");
	}
}

const LogicMgr::channel_t* LogicMgr::get_channel(
	const std::string& name) const
{
	for (const auto& chnl : _channels)
	{
		if (name == chnl.name)
			return &chnl;
	}
	return nullptr;	// not found.
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



