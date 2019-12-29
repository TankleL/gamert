#pragma once

#include "pre-req.hpp"
#include "singleton.hpp"

class VSceneGraph;
class LSceneGraph;

class ResMgrRuntime
	: public Singleton<ResMgrRuntime>
{
	DECL_SINGLETON_CTOR(ResMgrRuntime);

public:
	~ResMgrRuntime();

public:
	void manage_visual_scene(
		const std::string& name,
		VSceneGraph* vscene);

	void manage_logic_scene(
		const std::string& name,
		LSceneGraph* lscene);

	VSceneGraph* get_visual_scene(const std::string& name) const;
	LSceneGraph* get_logic_scene(const std::string& name) const;

	void purge_visual_scenes();
	void purge_logic_scenes();

private:
	std::unordered_map<
		std::string,
		VSceneGraph*>	_vscenes;
	std::unordered_map<
		std::string,
		LSceneGraph*>	_lscenes;
};

