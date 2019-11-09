#pragma once

#include "pre-req.hpp"

class VKRenderer;
class VDrawCall
{
public:
	VDrawCall(VKRenderer* renderer)
		: _renderer(renderer) {}
	virtual ~VDrawCall() {}

public:
	VKRenderer* get_renderer() const { return _renderer; }

protected:
	VKRenderer* _renderer;
};

