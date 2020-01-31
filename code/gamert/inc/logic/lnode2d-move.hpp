#pragma once

#include "lnode.hpp"
#include "vnode2d.hpp"
#include "opt-bindvnode.hpp"

class LNode2dMove
	: public LNode
	, public IOptBindVNode
{
public:
	LNode2dMove();
	virtual ~LNode2dMove();

public:
	void set_controller(int controller);

public:
	virtual void on_tick(const tick_param_t& param) override;
	virtual void bind_vnode(VNode* vnode) override;

private:
	VNode2d*		_vnode;
	int				_controller;
};
