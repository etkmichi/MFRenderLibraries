/*
 * MFVkCommandManager.h
 *
 *  Created on: 14.08.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKRESOURCES_MFVKCOMMANDMANAGER_H_
#define MFVULKAN_MFVKRESOURCES_MFVKCOMMANDMANAGER_H_

class MFVkCommandManager;

#include "MFObjects/MFObject.h"
#include "vulkan/vulkan.h"
#include "../MFVkContext/MFVkContext.h"
#include "../MFVkResources/MFVkResources.h"

class MFVkCommandManager {
private:
	MFVkContext
		*mp_context;

	MFVkResources
		*mp_resources;

	VkCommandPool
		*mp_commandPool;

	VkQueue
		*mp_graphicsQueue;

	std::vector<VkCommandBuffer>
		*mp_vecCommandBuffer;

	uint32_t
		m_bufferCount=2;
public:
	MFVkCommandManager(MFVkContext *pContext,MFVkResources *pResources);
	virtual ~MFVkCommandManager();
	/**
	 * This function disconnects the command buffers.
	 */
	void freeBuffer();
	std::vector<VkCommandBuffer>* getVecCommandBuffer();
	bool init();

	/**
	 * If the command buffer is used to draw, take care to setup the count of framebuffers with the same count.
	 * @param
	 */
	void setBufferCount(uint32_t count);

	/**
	 *
	 * @return
	 */
	bool createCommandPool();

	/**
	 * This method will create empty command buffers. The buffers must be filled in order to let the graphics card do something you want or may be u dont want it to do.
	 * Retrieves the count of swap chain images from MFVkResources and allocates the same ammount of command buffers.
	 * The next command buffer to use is (currentImageIndex + 1) % imageCount.
	 * @return
	 */
	bool createCommandBuffers();

	/**
	 * This command will create a new commandbuffer. After the command buffer was used, the endSingleTimeCommands(buffer) must be called with the returne buffer pointer as
	 * parameter. The commandbuffer will be deleted ind endSingleTimeCommands!
	 * @return
	 */
	VkCommandBuffer* beginSingleTimeCommands();
	VkCommandBuffer* beginRenderCommands();
	void endSingleTimeCommands(VkCommandBuffer *commandBuffer);
};
#endif /* MFVULKAN_MFVKRESOURCES_MFVKCOMMANDMANAGER_H_ */
