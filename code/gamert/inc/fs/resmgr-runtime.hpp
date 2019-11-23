#pragma once

#include "pre-req.hpp"
#include "singleton.hpp"

class VSceneGraph;
class LNode;

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
		LNode* lscene);

	VSceneGraph* get_visual_scene(const std::string& name) const;

	LNode* get_logic_scene(const std::string& name) const;

	void purge_visual_scenes();
	void purge_logic_scenes();

private:
	std::unordered_map<
		std::string,
		VSceneGraph*>	_vscenes;
	std::unordered_map<
		std::string,
		LNode*>			_lscenes;
};

