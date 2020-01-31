#pragma once

#include "pre-req.hpp"
#include "lnode.hpp"
#include "vnode2d.hpp"
#include "opt-bindvnode.hpp"

class LNode2dCharacterObserver
	: public LNode
	, public IOptBindVNode
{
public:
	LNode2dCharacterObserver();
	virtual ~LNode2dCharacterObserver();

public:
	void start_syncing();
	void stop_syncing();

public:
	virtual void on_tick(const tick_param_t& param) override;
	virtual void bind_vnode(VNode* vnode) override;

private:
	VNode2d*	_vnode;
};


