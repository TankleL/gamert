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
	typedef struct _st_channel
	{
		_st_channel()
			: name("default")
			, tick_itvl(0.0f)
			, scene(nullptr)
		{}

		std::string		name;
		float			tick_itvl;		// ticking interval in milliseconds
		LSceneGraph* scene;
	} channel_t;

public:
	void tick();

	void add_channel(
		const std::string& unique_name,
		float tick_interval,
		LSceneGraph* scene);
	const channel_t* get_channel(
		const std::string& name) const;

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

private:
	typedef struct _st_channel_internal
		: public channel_t
	{
		_st_channel_internal()
			: elapsed(0.0f)
			, ticks(0)
		{}

		float		elapsed;
		uint32_t	ticks;
	}_channel_internal_t;

	std::vector<_channel_internal_t>	_channels;
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
