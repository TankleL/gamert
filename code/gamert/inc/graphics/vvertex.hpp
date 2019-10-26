#pragma once

#include "pre-req.hpp"
#include "vvector.hpp"
#include "vulkan/vulkan.hpp"

class IVVertexDescriptor
{
public:
	virtual const VkVertexInputBindingDescription& binding_description() const = 0;
	virtual const std::vector<VkVertexInputAttributeDescription>& attribute_description() const = 0;
};

/**
*	@class VertexPC
*	@brief Position & Color Vertex
*/
class VVertexPC
{
public:
	VFVec2	pos;
	VFVec3	color;
};

class VVertexPCDescriptor : public IVVertexDescriptor
{
public:
	VVertexPCDescriptor();

public:
	virtual const VkVertexInputBindingDescription& binding_description() const override;
	virtual const std::vector<VkVertexInputAttributeDescription>& attribute_description() const override;

private:
	static VkVertexInputBindingDescription _s_binding_desc;
	static std::vector<VkVertexInputAttributeDescription> _s_attri_desc;
};


