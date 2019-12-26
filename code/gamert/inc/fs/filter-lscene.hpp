#pragma once

#include "pre-req.hpp"
#include "lnode.hpp"
#include "lscenegraph.hpp"

class _FilterLScene_XmlVisitor;

/**
 * @class FilterLScene
 * @brief Logic-Scene's serializer and deserializer.
 */
class FilterLScene
{
	friend _FilterLScene_XmlVisitor;
public:
	FilterLScene();

public:
	LSceneGraph* load(const std::string& filename);
	void save(
		const std::string& filename,
		LSceneGraph* scene);

private:
	void _load_scenegraph(void* xmlgraph);

private:
	LSceneGraph* _scene;
};

