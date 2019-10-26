#include "vvertex.hpp"

VkVertexInputBindingDescription					VVertexPCDescriptor::_s_binding_desc;
std::vector<VkVertexInputAttributeDescription>	VVertexPCDescriptor::_s_attri_desc;

VVertexPCDescriptor::VVertexPCDescriptor()
{
	_s_binding_desc.binding = 0;
	_s_binding_desc.stride = sizeof(VVertexPC);
	_s_binding_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	_s_attri_desc[0].binding = 0;
	_s_attri_desc[0].location = 0;
	_s_attri_desc[0].format = VK_FORMAT_R32G32_SFLOAT;
	_s_attri_desc[0].offset = offsetof(VVertexPC, pos);

	_s_attri_desc[1].binding = 0;
	_s_attri_desc[1].location = 1;
	_s_attri_desc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	_s_attri_desc[1].offset = offsetof(VVertexPC, color);
}

const VkVertexInputBindingDescription& VVertexPCDescriptor::binding_description() const
{
	return _s_binding_desc;
}

const std::vector<VkVertexInputAttributeDescription>& VVertexPCDescriptor::attribute_description() const
{
	return _s_attri_desc;
}
