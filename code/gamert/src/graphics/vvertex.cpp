#include "vvertex.hpp"

VkVertexInputBindingDescription					VVertex2DRGBDescriptor::_s_binding_desc;
std::vector<VkVertexInputAttributeDescription>	VVertex2DRGBDescriptor::_s_attri_desc;

VVertex2DRGBDescriptor::VVertex2DRGBDescriptor()
{
	_s_binding_desc.binding = 0;
	_s_binding_desc.stride = sizeof(VVertex2DRGB);
	_s_binding_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	_s_attri_desc[0].binding = 0;
	_s_attri_desc[0].location = 0;
	_s_attri_desc[0].format = VK_FORMAT_R32G32_SFLOAT;
	_s_attri_desc[0].offset = offsetof(VVertex2DRGB, pos);

	_s_attri_desc[1].binding = 0;
	_s_attri_desc[1].location = 1;
	_s_attri_desc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	_s_attri_desc[1].offset = offsetof(VVertex2DRGB, color);
}

const VkVertexInputBindingDescription& VVertex2DRGBDescriptor::binding_description() const
{
	return _s_binding_desc;
}

const std::vector<VkVertexInputAttributeDescription>& VVertex2DRGBDescriptor::attribute_description() const
{
	return _s_attri_desc;
}
