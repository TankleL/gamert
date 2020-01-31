#pragma once

#include "pre-req.hpp"
#include "lnode.hpp"
#include "vnode2d.hpp"

class LNode2dCharacterObserver : public LNode
{
public:
	LNode2dCharacterObserver();
	virtual ~LNode2dCharacterObserver();

public:
	void bind(VNode2d* node);
	void start_syncing();
	void stop_syncing();

public:
	virtual void on_tick(const tick_param_t& param) override;

private:
	VNode2d*	_vnode;
};


