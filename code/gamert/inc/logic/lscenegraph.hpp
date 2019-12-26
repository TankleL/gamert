#pragma once

#include "pre-req.hpp"

#include "lnode.hpp"

class LSceneGraph
{
public:
	LSceneGraph();
	virtual ~LSceneGraph();

public:
	LNode*	switch_root_node(LNode* new_node);
	LNode*	get_root_node() const;

	void	set_update_interval(float itvl);
	float	get_update_interval() const;

private:
	LNode*	_root_node;
	float	_update_itvl;		// update interval in milliseconds.

private:
	static LNode _dummy_node;
};

