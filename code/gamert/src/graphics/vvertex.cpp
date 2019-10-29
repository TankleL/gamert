#include "vvertex.hpp"

VkVertexInputBindingDescription					VVertex2DRGBDescriptor::_s_binding_desc;
std::vector<VkVertexInputAttributeDescription>	VVertex2DRGBDescriptor::_s_attri_desc;

VVertex2DRGBDescriptor::VVertex2DRGBDescriptor()
{
	_s_binding_desc.binding = 0;
	_s_binding_desc.stride = sizeof(VVertex2DRGB);
	_s_binding_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	{
		VkVertexInputAttributeDescription attr_desc;
		attr_desc.binding = 0;
		attr_desc.location = 0;
		attr_desc.format = VK_FORMAT_R32G32_SFLOAT;
		attr_desc.offset = offsetof(VVertex2DRGB, pos);
		_s_attri_desc.push_back(attr_desc);
	}

	{
		VkVertexInputAttributeDescription attr_desc;
		attr_desc.binding = 0;
		attr_desc.location = 1;
		attr_desc.format = VK_FORMAT_R32G32B32_SFLOAT;
		attr_desc.offset = offsetof(VVertex2DRGB, color);
		_s_attri_desc.push_back(attr_desc);
	}
}

const VkVertexInputBindingDescription& VVertex2DRGBDescriptor::binding_description() const
{
	return _s_binding_desc;
}

const std::vector<VkVertexInputAttributeDescription>& VVertex2DRGBDescriptor::attribute_description() const
{
	return _s_attri_desc;
}
