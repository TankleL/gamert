#include "vscenegraph.hpp"
#include "filter-vscene.hpp"
#include "tinyxml2.h"
#include "resmgr-static.hpp"
#include "vscenegraph2d.hpp"
#include "vnode-quad2d.hpp"
#include "vvector.hpp"
#include "utils-string.hpp"

class _FilterVScene_XmlVisitor_SceneGraph2d;

/********************************************************************
 * @class _FilterVScene_NodeProcTable
 * @singleton
 * @brief store a table of node processes
 *******************************************************************/
class _FilterVScene_NodeProcTable_SceneGraph2d
	: public Singleton<_FilterVScene_NodeProcTable_SceneGraph2d>
{
	DECL_SINGLETON_CTOR(_FilterVScene_NodeProcTable_SceneGraph2d);

public:
	typedef
		VNode* (_FilterVScene_XmlVisitor_SceneGraph2d::* node_proc_t)
		(const tinyxml2::XMLElement& element);

public:
	node_proc_t get_proc(
		const std::string& name) const
	{
		const auto& the_proc = _procs.find(name);
		if (the_proc != _procs.end())
		{
			return the_proc->second;
		}
		else
		{
			return nullptr;
		}
	}

private:
	std::unordered_map<
		std::string,
		node_proc_t>	_procs;
};


/********************************************************************
 * @class _FilterVScene_XmlVisitor
 * @brief help filter process xml nodes
 *******************************************************************/
class _FilterVScene_XmlVisitor_SceneGraph2d
	: public tinyxml2::XMLVisitor
{
	friend _FilterVScene_NodeProcTable_SceneGraph2d;
public:
	_FilterVScene_XmlVisitor_SceneGraph2d(FilterVScene& filter)
		: _filter(filter)
		, _curnode(nullptr)
	{}

public:
	/// Visit an element.
	virtual bool VisitEnter(
		const tinyxml2::XMLElement& element,
		const tinyxml2::XMLAttribute* firstAttribute)  override
	{
		_FilterVScene_NodeProcTable_SceneGraph2d::node_proc_t proc =
			_FilterVScene_NodeProcTable_SceneGraph2d::get_instance()
			.get_proc(element.Name());

		if (proc)
		{
			VNode* newnode = (this->*proc)(element);
			if (newnode)
			{
				if(_curnode)
					_curnode->manage_child(newnode);
				_curnode = newnode;
			}
		}
		return true;
	}

	/// Visit an element.
	virtual bool VisitExit(
		const tinyxml2::XMLElement& element) override
	{
		if (_curnode && _curnode->get_parent())
		{
			_curnode = _curnode->get_parent();
		}
		return true;
	}

public:
	VNode* graphroot() const
	{
		return _curnode;
	}

private:
	VNode* _proc_graphroot(
		const tinyxml2::XMLElement& element)
	{
		return new VNode2d();
	}

	VNode* _proc_node_quad(
		const tinyxml2::XMLElement& element)
	{
		VNodeQuad2d* result = new VNodeQuad2d();

		const char* name = element.Attribute("Name");
		if (name)
		{
			result->set_name(name);
		}

		const char* scale = element.Attribute("Scale");
		if (scale)
		{
			std::vector<float> param =
				utils_string::strings_to_floats(scale, ',');
			
			if (param.size() == 2)
			{
				result->set_scale_fast(VFVec2({
					param[0],
					param[1]}));
			}
			else
			{
				throw std::runtime_error("bad scale attribute.");
			}
		}

		const char* position = element.Attribute("Position");
		if (position)
		{
			std::vector<float> param =
				utils_string::strings_to_floats(position, ',');

			if (param.size() == 2)
			{
				result->set_poisition_fast(VFVec2({
					param[0],
					param[1] }));
			}
			else
			{
				throw std::runtime_error("bad position attribute.");
			}
		}

		result->calculate_world();
		return result;
	}

private:
	FilterVScene&	_filter;
	VNode*			_curnode;
};


/********************************************************************
 * @ctor _FilterVScene_NodeProcTable
 * @brief intitialize process table
 *******************************************************************/
_FilterVScene_NodeProcTable_SceneGraph2d
::_FilterVScene_NodeProcTable_SceneGraph2d()
{
	_procs["GraphRoot"] = &_FilterVScene_XmlVisitor_SceneGraph2d
		::_proc_graphroot;
	_procs["Node-Quad"] = &_FilterVScene_XmlVisitor_SceneGraph2d
		::_proc_node_quad;
}


/********************************************************************
 * @impl FilterVScene
 *******************************************************************/
FilterVScene::FilterVScene()
	: _scene(nullptr)
{}

VSceneGraph* FilterVScene::load(const std::string& filename)
{
	try
	{
		tinyxml2::XMLDocument xdoc;
		xdoc.LoadFile(
			ResMgrStatic::get_instance()
			.fullpath(filename)
			.c_str());

		const auto& xgraph = xdoc.RootElement();

		if (GRT_IS_STRING_EQUAL(
			xgraph->Name(),
			"SceneGraph2d"))
		{
			_load_scenegraph2d(xgraph);
			return _scene;
		}
		else
		{
			throw std::runtime_error(
				"unknown file format.");
		}
	}
	catch (std::exception ex)
	{
		// TODO: handle exceptions
		throw ex;
	}
}

void FilterVScene::save(
	const std::string& filename,
	VSceneGraph* scene)
{}

void FilterVScene::_load_scenegraph2d(void* xmlgraph)
{
	VSceneGraph2d* scene = new VSceneGraph2d();
	tinyxml2::XMLElement* xgraph = (tinyxml2::XMLElement*)xmlgraph;
	
	_scene = scene;
	
	try
	{
		tinyxml2::XMLElement* xroot = xgraph->LastChildElement("GraphRoot");
		if (xroot != nullptr)
		{
			_FilterVScene_XmlVisitor_SceneGraph2d visitor(*this);
			xroot->Accept(&visitor);
			if (visitor.graphroot() != nullptr)
			{
				scene->init();
				scene->switch_root_node(visitor.graphroot());
			}
		}
		else
		{
			throw std::runtime_error("GraphRoot not found.");
		}
	}
	catch (std::exception ex)
	{
		delete _scene;
		_scene = nullptr;
		throw ex;
	}
}

