#pragma once
#include "vdrawcall.hpp"

class VDrawCall2d : public VDrawCall
{
public:
	static const uint32_t	Invalid_Offset;

public:
	VDrawCall2d(VKRenderer* renderer);
	virtual ~VDrawCall2d();

public:
	uint32_t	get_single_dc_ubo_offset() const;

private:
	void _create();
	void _destroy();

private:
	uint32_t	_offset_single_dc;
};
