#include "vnode.hpp"
#include "utils-string.hpp"

using namespace std;

VNode::VNode()
	: _scene(nullptr)
	, _parent(nullptr)
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

const std::string& VNode::get_name() const
{
	return _name;
}

void VNode::manage_child(VNode* child)
{
	assert(child->is_managed() == false);

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
		on_render(param);
		for (const auto& child : _children)
		{
			child->render(param);
		}
	}
}

void VNode::init(VSceneGraph* scene)
{
	GRT_CHECK(
		nullptr == _scene || scene == _scene,
		"the incoming scene must be null, new or same.");

	if (scene)
	{
		_scene = scene;
		this->on_init();
		for (auto& child : _children)
		{
			child->init(scene);
		}
	}
}

void VNode::uninit()
{
	for (auto& child : _children)
	{
		child->uninit();
	}
	this->on_uninit();
	_scene = nullptr;
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
		_drawcall = nullptr;
	}
}

VNode* VNode::get_child_flat(const std::string& name) const
{
	for (const auto& child : _children)
	{
		if (name == child->get_name())
			return child;
	}
	return nullptr;
}

VNode* VNode::get_child(const std::string& name) const
{
	return _get_child(name, 0);
}

VNode* VNode::_get_child(
	const std::string& name,
	size_t strview_beg) const
{
	for (const auto& child : _children)
	{
		if (utils_string
			::are_strings_equal(
				child->get_name(),
				name,
				strview_beg, '/'))
		{
			if (strview_beg + child->get_name().length() + 1
				< name.length())
			{
				return child->_get_child(
					name,
					strview_beg + child->get_name().length() + 1);
			}
			else
			{
				return child;
			}
		}
	}

	return nullptr;
}
