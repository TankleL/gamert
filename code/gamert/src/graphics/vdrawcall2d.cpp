#include "vulkan/vulkan.hpp"
#include "vdrawcall2d.hpp"
#include "vkrenderer2d.hpp"

const uint32_t VDrawCall2d::Invalid_Offset = (uint32_t)-1;

VDrawCall2d::VDrawCall2d(VKRenderer* renderer)
	: VDrawCall(renderer)
	, _offset_single_dc(Invalid_Offset)
{
	_create();
}

VDrawCall2d::~VDrawCall2d()
{
	_destroy();
}

void VDrawCall2d::_create()
{
	VKRenderer2d* rdr = dynamic_cast<VKRenderer2d*>(_renderer);
	if (rdr)
	{
		_offset_single_dc = rdr->allocate_single_dc_ubo();
	}
	else
	{
		throw std::runtime_error("not using a 2d renderer");
	}
}

void VDrawCall2d::_destroy()
{
	if (Invalid_Offset != _offset_single_dc)
	{
		VKRenderer2d* rdr = dynamic_cast<VKRenderer2d*>(_renderer);
		if (rdr)
		{
			rdr->free_single_dc_ubo(_offset_single_dc);
		}
		else
		{
			throw std::runtime_error("not using a 2d renderer");
		}
	}
}

uint32_t VDrawCall2d::get_single_dc_ubo_offset() const
{
	return _offset_single_dc;
}
