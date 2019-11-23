#pragma once

#include "pre-req.hpp"
#include "lnode.hpp"

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
	void bind_root(LNode* root);
	void load(const std::string& filename);
	void save(const std::string& filename);

private:
	LNode* _root;
};

