#include "singleton.hpp"
#include "filter-lscene.hpp"
#include "tinyxml2.h"
#include "resmgr-static.hpp"
#include "logicmgr.hpp"

class _FilterLScene_XmlVisitor;
class _FilterLScene_NodeProcTable
	: public Singleton<_FilterLScene_NodeProcTable>
{
	DECL_SINGLETON_CTOR(_FilterLScene_NodeProcTable);
	
public:
	typedef void(_FilterLScene_XmlVisitor::*node_proc_t)
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

class _FilterLScene_XmlVisitor
	: public tinyxml2::XMLVisitor
{
	friend _FilterLScene_NodeProcTable;
public:
	_FilterLScene_XmlVisitor(FilterLScene& filter)
		: _filter(filter)
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
			(this->*proc)(element);
		}
		return true;
	}

	/// Visit an element.
	virtual bool VisitExit(
		const tinyxml2::XMLElement& element) override 
	{
		return true;
	}

private:
	void _proc_lnode2d_move(
		const tinyxml2::XMLElement& element)
	{
		LNode* node = LogicMgr::get_instance().create_lnode(
			element.Name());
		GRT_CHECK(
			node != nullptr,
			"logic manager doesn't know how to create target node");
		_filter._root->manage_child(node);
	}

private:
	FilterLScene&		_filter;
};

_FilterLScene_NodeProcTable::_FilterLScene_NodeProcTable()
{
	_procs["Node2d-Move"] = &_FilterLScene_XmlVisitor::_proc_lnode2d_move;
}

FilterLScene::FilterLScene()
	: _root(nullptr)
{}

void FilterLScene::bind_root(LNode* root)
{
	_root = root;
}

void FilterLScene::load(const std::string& filename)
{
	try
	{
		tinyxml2::XMLDocument xdoc;
		xdoc.LoadFile(
			ResMgrStatic::get_instance()
			.fullpath(filename)
			.c_str());

		const auto& root = xdoc.RootElement();
		GRT_CHECK(
			GRT_IS_STRING_EQUAL(
				root->Name(),
				"root"),
			"unknown file format");

		_FilterLScene_XmlVisitor visitor(*this);
		root->Accept(&visitor);
	}
	catch (std::exception ex)
	{
		// TODO: handle exceptions
		throw ex;
	}
}

void FilterLScene::save(const std::string& filename)
{}

