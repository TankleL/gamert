#include "lscenegraph.hpp"

LNode LSceneGraph::_dummy_node;

LSceneGraph::LSceneGraph()
	: _root_node(&_dummy_node)
	, _update_itvl(100.f)
{}

LSceneGraph::~LSceneGraph()
{}

LNode* LSceneGraph::switch_root_node(LNode* new_node)
{
	if (nullptr == new_node)
		new_node = &_dummy_node;

	LNode* old = _root_node;
	// old->uninit();
	// new_node->init(this);
	_root_node = new_node;
	return old;
}

LNode* LSceneGraph::get_root_node() const
{
	return _root_node;
}

void LSceneGraph::set_update_interval(float itvl)
{
	_update_itvl = itvl;
}

float LSceneGraph::get_update_interval() const
{
	return _update_itvl;
}



