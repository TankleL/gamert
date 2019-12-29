#pragma once

#include "pre-req.hpp"

/**
 * @class LNode
 * @brief Logic node.
 */
class LNode
{
public:
	typedef struct _struct_tick_param
	{
		float		elapsed;	// millisecond
		uint32_t	ticks;		// logic tick
	} tick_param_t;

public:
	LNode();
	virtual ~LNode();

public:
	virtual void on_managed() {};
	virtual void on_tick(const tick_param_t& param) {};
	virtual void on_detached() {};

public:
	bool		is_managed() const;

	void				set_name(const std::string& name);
	const std::string&	get_name() const;
	LNode*				get_parent() const;

	void	tick(const tick_param_t& param);
	void	manage_child(LNode* child);
	LNode*	detach_child(const std::string& name);
	void	detach_child(LNode* node);
	void	detach();

private:
	std::vector<LNode*>		_children;
	std::string				_name;
	LNode*					_parent;
};

