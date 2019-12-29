#include "resmgr-runtime.hpp"
#include "vscenegraph.hpp"
#include "lscenegraph.hpp"

ResMgrRuntime::ResMgrRuntime()
{}

ResMgrRuntime::~ResMgrRuntime()
{}

void ResMgrRuntime::manage_visual_scene(
	const std::string& name,
	VSceneGraph* vscene)
{
	_vscenes[name] = vscene;
}

void ResMgrRuntime::manage_logic_scene(
	const std::string& name,
	LSceneGraph* lscene)
{
	_lscenes[name] = lscene;
}

VSceneGraph*
ResMgrRuntime::get_visual_scene(const std::string& name) const
{
	const auto& scene = _vscenes.find(name);
	if (scene != _vscenes.end())
	{
		return scene->second;
	}
	else
	{
		return nullptr;
	}
}

LSceneGraph*
ResMgrRuntime::get_logic_scene(const std::string& name) const
{
	const auto& scene = _lscenes.find(name);
	GRT_CHECK(
		scene != _lscenes.end(),
		"target scene not found.");

	return scene->second;
}

void ResMgrRuntime::purge_visual_scenes()
{
	for (auto& vscene : _vscenes)
	{
		delete vscene.second;
	}
	_vscenes.clear();
}

void ResMgrRuntime::purge_logic_scenes()
{
	for (auto& lscene : _lscenes)
	{
		delete lscene.second;
	}
	_lscenes.clear();
}
