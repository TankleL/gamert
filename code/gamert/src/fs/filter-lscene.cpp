#include "singleton.hpp"
#include "filter-lscene.hpp"
#include "tinyxml2.h"
#include "resmgr-static.hpp"
#include "resmgr-runtime.hpp"
#include "logicmgr.hpp"
#include "vscenegraph.hpp"
#include "lnode2d-move.hpp"

class _FilterLScene_XmlVisitor;


/********************************************************************
 * @class _FilterLScene_NodeProcTable
 * @singleton
 * @brief store a table of node processes
 *******************************************************************/
class _FilterLScene_NodeProcTable
	: public Singleton<_FilterLScene_NodeProcTable>
{
	DECL_SINGLETON_CTOR(_FilterLScene_NodeProcTable);
	
public:
	typedef
		LNode* (_FilterLScene_XmlVisitor::*node_proc_t)
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
 * @class _FilterLScene_XmlVisitor
 * @brief help filter process xml nodes
 *******************************************************************/
class _FilterLScene_XmlVisitor
	: public tinyxml2::XMLVisitor
{
	friend _FilterLScene_NodeProcTable;
public:
	_FilterLScene_XmlVisitor(FilterLScene& filter)
		: _filter(filter)
		, _curnode(nullptr)
	{}

public:
	/// Visit an element.
	virtual bool VisitEnter(
		const tinyxml2::XMLElement& element,
		const tinyxml2::XMLAttribute* firstAttribute)  override 
	{
		_FilterLScene_NodeProcTable::node_proc_t proc =
			_FilterLScene_NodeProcTable::get_instance()
				.get_proc(element.Name());

		if (proc)
		{
			LNode* newnode = ((this->*proc))(element);
			if (newnode)
			{
				if (_curnode)
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
	LNode* graphroot() const
	{
		return _curnode;
	}

private:
	LNode* _proc_graphroot(
		const tinyxml2::XMLElement& element)
	{
		return new LNode();
	}

	LNode* _proc_lnode2d_move(
		const tinyxml2::XMLElement& element)
	{
		LNode2dMove* node = (LNode2dMove*)LogicMgr::get_instance().create_lnode(
			element.Name());
		GRT_CHECK(
			node != nullptr,
			"logic manager doesn't know how to create target node");

		const char* name = element.Attribute("Name");
		if (name)
		{
			node->set_name(name);
		}

		const char* bind_vscene = element.Attribute("BindVScene");
		const char* bind_vnode = element.Attribute("BindVNode");
		if (bind_vscene && bind_vnode)
		{
			VSceneGraph* vscene = ResMgrRuntime::get_instance()
				.get_visual_scene(bind_vscene);
			if (!vscene)
			{
				delete node;
				throw std::runtime_error(
					"the given BindVScene not found.");
			}

			VNode* vroot = vscene->get_root_node();
			if (!vroot)
			{
				delete node;
				throw std::runtime_error(
					"the given BindVScene doesn't have root node.");
			}

			VNode* target = (VNode*)vroot->get_child(bind_vnode);
			if (target)
			{
				if (GRT_IS_CLASS_PTR(target, VNode2d))
				{
					node->bind((VNode2d*)target);
				}
				else
				{
					delete node;
					throw std::runtime_error(
						"the given BindVNode is not a 2d node.");
				}
			}
			else
			{
				delete node;
				throw std::runtime_error("bad vscene params.");
			}
		}
		else
		{
			delete node;
			throw std::runtime_error("bad vscene params.");
		}

		return node;
	}

private:
	FilterLScene&		_filter;
	LNode*				_curnode;
};


/********************************************************************
 * @ctor _FilterVScene_NodeProcTable
 * @brief intitialize process table
 *******************************************************************/
_FilterLScene_NodeProcTable::_FilterLScene_NodeProcTable()
{
	_procs["GraphRoot"] = &_FilterLScene_XmlVisitor::_proc_graphroot;
	_procs["Node2d-Move"] = &_FilterLScene_XmlVisitor::_proc_lnode2d_move;
}


/********************************************************************
 * @impl FilterLScene
 *******************************************************************/
FilterLScene::FilterLScene()
	: _scene(nullptr)
{}

LSceneGraph* FilterLScene::load(const std::string& filename)
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
			"SceneGraph"))
		{
			_load_scenegraph(xgraph);
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

void FilterLScene::save(
	const std::string& filename,
	LSceneGraph* scene)
{}

void FilterLScene::_load_scenegraph(void* xmlgraph)
{
	tinyxml2::XMLElement* xgraph = (tinyxml2::XMLElement*)xmlgraph;
	_scene = new LSceneGraph();

	try
	{
		tinyxml2::XMLElement* xroot = xgraph->LastChildElement("GraphRoot");
		if (xroot != nullptr)
		{
			_FilterLScene_XmlVisitor visitor(*this);
			xroot->Accept(&visitor);
			if (visitor.graphroot() != nullptr)
			{
				_scene->switch_root_node(visitor.graphroot());
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
