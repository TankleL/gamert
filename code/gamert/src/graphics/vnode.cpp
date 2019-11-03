#include "vnode.hpp"

using namespace std;

VNode::VNode()
	: _visible(true)
{}

VNode::~VNode()
{
	for (auto& child : _children)
	{
		delete child;
	}
}

void VNode::set_name(const std::string& name)
{
	_name = name;
}

std::string VNode::get_name() const
{
	return _name;
}

void VNode::attach_child(VNode* child)
{
	_children.push_back(child);
}

VNode* VNode::detach_child(const std::string& name)
{
	VNode* result = nullptr;

	for (auto iter_child = _children.begin();
			iter_child != _children.end();
			++iter_child)
	{
		if ((*iter_child)->get_name() == name)
		{
			result = *iter_child;
			_children.erase(iter_child);
			break;
		}
	}

	return result;
}

void VNode::render(const render_param_t& param)
{
	on_prerender();
	for (const auto& child : _children)
	{
		child->on_prerender();
	}

	on_render(param);
	for (const auto& child : _children)
	{
		child->on_render(param);
	}

	on_postrender();
	for (const auto& child : _children)
	{
		child->on_postrender();
	}
}

void VNode::init()
{
	this->on_init();
	for (auto& child : _children)
	{
		child->init();
	}
}

void VNode::uninit()
{
	for (auto& child : _children)
	{
		child->uninit();
	}
	this->on_uninit();
}

