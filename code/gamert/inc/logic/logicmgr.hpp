#pragma once

#include "pre-req.hpp"
#include "singleton.hpp"
#include "ltimer.hpp"
#include "lscenegraph.hpp"

class LogicMgr : public Singleton<LogicMgr>
{
	DECL_SINGLETON_CTOR(LogicMgr);

public:
	typedef std::function<LNode* (void)> creator_t;

public:
	void tick();

	LSceneGraph* switch_scene_graph(LSceneGraph* graph);
	LSceneGraph* get_scene_graph() const;

	void register_lnode_creator(
		const std::string& name,
		const creator_t& creator);
	void unregister_lnode_creators();

	LNode* create_lnode(const std::string& name);

private:
	std::unordered_map<
		std::string,
		creator_t> _lnode_creator;

private:
	LTimer			_timer;
	uint32_t		_tick;

private:
	LSceneGraph*	_scene;

private:
	static LSceneGraph _dummy_scene;
};


template<class _NodeType>
class LNodeCreatorRegister
{
public:
	LNodeCreatorRegister(const std::string& name)
	{
		LogicMgr::get_instance().register_lnode_creator(
			name,
			[]()-> LNode* {
				return new _NodeType();
			});
	}
};

#define REG_LND_CREATOR(type, name)	\
			LNodeCreatorRegister<type> _iv_reg_lnode_##type(name)
