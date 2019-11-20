#include "lnode.hpp"

LNode::LNode()
	: _parent(nullptr)
{}

LNode::~LNode()
{
	for (auto& child : _children)
	{
		delete child;
	}
}

bool LNode::is_managed() const
{
	return _parent != nullptr;
}

void LNode::set_name(const std::string& name)
{
	_name = name;
}

const std::string& LNode::get_name() const
{
	return _name;
}

void LNode::tick()
{
	on_tick();
	for (auto& child : _children)
	{
		child->tick();
	}
}

void LNode::manage_child(LNode* child)
{
	assert(child->is_managed() == false);

	child->_parent = this;
	_children.push_back(child);

	child->on_managed();
}

LNode* LNode::detach_child(const std::string& name)
{
	LNode* result = nullptr;

	for (auto iter_child = _children.begin();
		iter_child != _children.end();
		++iter_child)
	{
		if ((*iter_child)->get_name() == name)
		{
			result = *iter_child;
			result->_parent = nullptr;
			result->on_detached();
			_children.erase(iter_child);
			break;
		}
	}

	return result;
}

void LNode::detach_child(LNode* node)
{
	for (auto iter_child = _children.begin();
		iter_child != _children.end();
		++iter_child)
	{
		if ((*iter_child) == node)
		{
			node->_parent = nullptr;
			node->on_detached();
			_children.erase(iter_child);
			break;
		}
	}
}

void LNode::detach()
{
	if (_parent)
	{
		_parent->detach_child(this);
	}
}




