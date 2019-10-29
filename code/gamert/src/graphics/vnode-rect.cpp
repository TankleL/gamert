//#include "vnode-rect.hpp"
//#include "vkutils.hpp"
//
//void VNodeRect::on_init()
//{
//	_vertices[0].pos = VFVec2({ 0.0f, -0.5f });
//	_vertices[0].color = VFVec3({ 1.0f, 1.0f, 1.0f });
//
//	_vertices[1].pos = VFVec2({ 0.5f, 0.5f });
//	_vertices[1].color = VFVec3({ 0.0f, 1.0f, 0.0f });
//
//	_vertices[2].pos = VFVec2({ -0.5f, 0.5f });
//	_vertices[2].color = VFVec3({ 0.0f, 0.0f, 1.0f });
//
//	RenderMgr& rmgr = RenderMgr::get_instance();
//
//	VkUtils::create_buffer(
//		_vertex_buffer,
//		_vertex_dev_mem,
//		rmgr.get_vulkan_device(),
//		rmgr.get_vulkan_physical_device(),
//		sizeof(VVertex2DRGB) * 3,
//		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
//		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
//
//	void* mapdata;
//	vkMapMemory(
//		rmgr.get_vulkan_device(),
//		_vertex_dev_mem,
//		0,
//		sizeof(VVertex2DRGB) * 3,
//		0,
//		&mapdata);
//	memcpy(mapdata, _vertices, sizeof(VVertex2DRGB) * 3);
//	vkUnmapMemory(rmgr.get_instance().get_vulkan_device(), _vertex_dev_mem);
//
//	VkCommandBufferAllocateInfo alloc_info = {};
//	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//	alloc_info.commandPool = rmgr.get_vulkan_command_pool();
//	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//	alloc_info.commandBufferCount = 1;
//
//	if (vkAllocateCommandBuffers(rmgr.get_vulkan_device(), &alloc_info, &_cmd) != VK_SUCCESS)
//	{
//		throw std::runtime_error("failed to allocate command buffers!");
//	}
//}
//
//void VNodeRect::on_render(std::vector<VkCommandBuffer>& cmdqueue, VkFramebuffer frmbuffer, VkCommandPool cmdpool)
//{
//	VkCommandBufferBeginInfo begin_info = {};
//	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//	
//	if (vkBeginCommandBuffer(_cmd, &begin_info) != VK_SUCCESS)
//	{
//		throw std::runtime_error("failed to begin recording command buffer!");
//	}
//
//	RenderMgr& rmgr = RenderMgr::get_instance();
//	VkRenderPassBeginInfo render_pass_info = {};
//	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//	render_pass_info.renderPass = rmgr.get_vulkan_render_pass();
//	render_pass_info.framebuffer = frmbuffer;
//	render_pass_info.renderArea.offset = { 0, 0 };
//	render_pass_info.renderArea.extent = rmgr.get_vulkan_swapchain_extent();
//
//	vkCmdBeginRenderPass(_cmd, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
//
//	vkCmdBindPipeline(
//		_cmd,
//		VK_PIPELINE_BIND_POINT_GRAPHICS,
//		rmgr.get_vulkan_pipeline(RenderMgr::BTPL_2D_Position_RGBColor));
//
//	VkBuffer bind_bufferlist[] = { _vertex_buffer };
//	VkDeviceSize offsets[] = { 0 };
//	vkCmdBindVertexBuffers(_cmd, 0, 1, bind_bufferlist, offsets);
//
//	vkCmdDraw(_cmd, 3, 1, 0, 0);
//
//	vkCmdEndRenderPass(_cmd);
//
//	if (vkEndCommandBuffer(_cmd) != VK_SUCCESS)
//	{
//		throw std::runtime_error("failed to record command buffer!");
//	}
//
//	cmdqueue.push_back(_cmd);
//}
//
