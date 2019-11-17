#pragma once
#include "pre-req.hpp"
#include "vnode2d.hpp"
#include "vvertex.hpp"

class VNodeQuad2d : public VNode2d
{
public:
	VNodeQuad2d();
	virtual ~VNodeQuad2d();

public:
	virtual void on_init() override;
	virtual void on_create_drawcall(VKRenderer* renderer) override;
	virtual void on_render(const render_param_t& param) override;
	virtual void on_destroy_drawcall() override;
	virtual void on_uninit() override;
};
