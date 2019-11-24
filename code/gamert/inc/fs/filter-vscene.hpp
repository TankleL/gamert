#pragma once

#include "pre-req.hpp"
#include "lnode.hpp"

class VSceneGraph;
class _FilterVScene_XmlVisitor_SceneGraph2d;

/**
 * @class FilterLScene
 * @brief Logic-Scene's serializer and deserializer.
 */
class FilterVScene
{
	friend _FilterVScene_XmlVisitor_SceneGraph2d;
public:
	FilterVScene();

public:
	VSceneGraph* load(const std::string& filename);
	void save(
		const std::string& filename,
		VSceneGraph* scene);

private:
	void _load_scenegraph2d(void* xmlgraph);

private:
	VSceneGraph* _scene;
};
