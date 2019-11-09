#include "vnode.hpp"

using namespace std;

VNode::VNode()
	: _parent(nullptr)
	, _drawcall(nullptr)
	, _visible(true)
{}

VNode::~VNode()
{
	for (auto& child : _children)
	{
		delete child;
	}
}

bool VNode::is_managed() const
{
	return _parent != nullptr;
}

void VNode::set_name(const std::string& name)
{
	_name = name;
}

std::string VNode::get_name() const
{
	return _name;
}

void VNode::manage_child(VNode* child)
{
	child->_parent = this;
	_children.push_back(child);

	child->on_managed();
	if (_drawcall)
	{
		child->create_drawcall(_drawcall->get_renderer());
	}
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
			result->_parent = nullptr;
			result->destroy_drawcall();
			result->on_detached();
			_children.erase(iter_child);
			break;
		}
	}

	return result;
}

void VNode::detach_child(VNode* node)
{
	for (auto iter_child = _children.begin();
		iter_child != _children.end();
		++iter_child)
	{
		if ((*iter_child) == node)
		{
			node->_parent = nullptr;
			node->destroy_drawcall();
			node->on_detached();
			_children.erase(iter_child);
			break;
		}
	}
}

void VNode::detach()
{
	if (_parent)
	{
		_parent->detach_child(this);
	}
}

void VNode::render(const render_param_t& param)
{
	if (_visible)
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

void VNode::create_drawcall(VKRenderer* renderer)
{
	if (!_drawcall)
	{
		on_create_drawcall(renderer);

		for (auto& child : _children)
		{
			child->create_drawcall(renderer);
		}
	}
}

void VNode::destroy_drawcall()
{
	if (_drawcall)
	{
		for (auto& child : _children)
		{
			child->destroy_drawcall();
		}

		on_destroy_drawcall();
	}
}
