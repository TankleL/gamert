#include "lnode2d-character-observer.hpp"
#include "logicmgr.hpp"

REG_LND_CREATOR(LNode2dCharacterObserver, "Node2d-Character-Observer");

LNode2dCharacterObserver::LNode2dCharacterObserver()
	: _vnode(nullptr)
{}

LNode2dCharacterObserver::~LNode2dCharacterObserver()
{}

void LNode2dCharacterObserver::bind(VNode2d* node)
{
	_vnode = node;
}

void LNode2dCharacterObserver::on_tick(const tick_param_t& param)
{

}

void LNode2dCharacterObserver::start_syncing()
{}

void LNode2dCharacterObserver::stop_syncing()
{}

