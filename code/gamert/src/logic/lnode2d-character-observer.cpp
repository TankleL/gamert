#include "lnode2d-character-observer.hpp"
#include "logicmgr.hpp"

REG_LND_CREATOR(LNode2dCharacterObserver, "Node2d-Character-Observer");

LNode2dCharacterObserver::LNode2dCharacterObserver()
	: _vnode(nullptr)
{}

LNode2dCharacterObserver::~LNode2dCharacterObserver()
{}

void LNode2dCharacterObserver::bind_vnode(VNode* node)
{
#if defined(DEBUG) || defined(_DEBUG)
	VNode2d* check_node = dynamic_cast<VNode2d*>(node);
	GRT_CHECK(
		check_node != nullptr,
		"the node is not a 2d node")
	_vnode = check_node;
#else
	_vnode = static_cast<VNode2d*>(node);
#endif
}

void LNode2dCharacterObserver::on_tick(const tick_param_t& param)
{

}

void LNode2dCharacterObserver::start_syncing()
{}

void LNode2dCharacterObserver::stop_syncing()
{}

