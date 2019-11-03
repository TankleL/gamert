#pragma once

#include "pre-req.hpp"
#include "vvector.hpp"
#include "vulkan/vulkan.hpp"
#include "singleton.hpp"

class IVVertexDescriptor
{
public:
	virtual const VkVertexInputBindingDescription& binding_description() const = 0;
	virtual const std::vector<VkVertexInputAttributeDescription>& attribute_description() const = 0;
};

/**
*	@class VVertex2DRGB
*	@brief 2D Position & RGB Color Vertex
*/
class VVertex2DRGB
{
public:
	VVertex2DRGB();
	VVertex2DRGB(const VFVec2& position, const VFVec3& color);

public:
	VFVec2	pos;
	VFVec3	clr;
};

class VVertex2DRGBDescriptor
	: public Singleton<VVertex2DRGBDescriptor>
	, public IVVertexDescriptor
{
	DECL_SINGLETON_CTOR(VVertex2DRGBDescriptor);

public:
	virtual const VkVertexInputBindingDescription& binding_description() const override;
	virtual const std::vector<VkVertexInputAttributeDescription>& attribute_description() const override;

private:
	static VkVertexInputBindingDescription _s_binding_desc;
	static std::vector<VkVertexInputAttributeDescription> _s_attri_desc;
};


