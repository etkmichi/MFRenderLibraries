/*
 * MFVkCommandManager.cpp
 *
 *  Created on: 14.08.2019
 *      Author: michl
 */

#include "MFVkCommandManager.h"

MFVkCommandManager::MFVkCommandManager(MFVkContext *pContext,MFVkResources *pResources) {
	mp_context=pContext;
	mp_resources=pResources;
	mp_graphicsQueue = nullptr;
	mp_commandPool = new VkCommandPool();
	mp_vecCommandBuffer = new std::vector<VkCommandBuffer>();
	m_bufferCount=mp_context->getSetup()->m_swapChainImageCount;
}

MFVkCommandManager::~MFVkCommandManager() {
	mp_context->printInfo("Deleting MFVkCommandManager object!");
	freeBuffer();
	delete mp_vecCommandBuffer;
	vkDestroyCommandPool(*mp_context->getVirtualDevice(),*mp_commandPool,nullptr);
	delete mp_commandPool;
}
void MFVkCommandManager::freeBuffer(){
	vkFreeCommandBuffers(
			*mp_context->getVirtualDevice(),
			*mp_commandPool,mp_vecCommandBuffer->size(),
			mp_vecCommandBuffer->data());
}

std::vector<VkCommandBuffer>* MFVkCommandManager::getVecCommandBuffer(){
	return mp_vecCommandBuffer;
}

bool MFVkCommandManager::init(){
	mp_graphicsQueue = mp_context->getDevice()->getGraphicsQueue();
	if(mp_graphicsQueue == nullptr){
		mp_context->printErr("No graphics queue!");
		return false;
	}
	if(!createCommandPool()){
		mp_context->printErr("MFVkCommandManager::init() - failed to create command pool");
		return false;
	}
	if(!createCommandBuffers()){
		mp_context->printErr("MFVkCommandManager::init() - failed to create command buffers");
		return false;
	}
	return true;
}

bool MFVkCommandManager::createCommandPool(){
	VkCommandPoolCreateInfo poolInfo={};
	mp_context->printInfo("MFVkCommandManager::createCommandPool - "
	    "creating command pool");
	poolInfo.sType=VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags=VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = 0;//;mp_context->getDevice()->getGraphicsQueueIndices()->at(0);//TODO queue index VkQueue struct
	/*TODO maybe select queue function TODO get it from resources*/
	if (vkCreateCommandPool(*mp_context->getVirtualDevice(), &poolInfo, nullptr, mp_commandPool) != VK_SUCCESS) {
		mp_context->printErr("MFVkCommandManager - failed to create graphics command pool!");
		return false;
	}
	return true;
}

bool MFVkCommandManager::createCommandBuffers(){
	mp_vecCommandBuffer->resize(m_bufferCount);
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = *mp_commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t) mp_vecCommandBuffer->size();
	mp_context->printInfo("MFVkCommandManager::createCommandBuffers - Allocating command - "
			"buffer count: "+std::to_string(allocInfo.commandBufferCount));
	if (vkAllocateCommandBuffers(
			*mp_context->getVirtualDevice(),
			&allocInfo,
			mp_vecCommandBuffer->data()) != VK_SUCCESS) {
		mp_context->printErr("MFVkCommandManager::createCommandBuffers - "
		    "failed to allocate command buffers!");
		return false;
	}
  mp_context->printInfo("MFVkCommandManager::createCommandBuffers - "
      "Allocating command buffer done");
	return true;
}
VkCommandBuffer* MFVkCommandManager::beginSingleTimeCommands(){
	VkCommandBufferAllocateInfo allocInfo={};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = *mp_commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer* commandBuffer=new VkCommandBuffer();
	vkAllocateCommandBuffers(
			*(mp_context->getVirtualDevice()),&allocInfo,commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(*commandBuffer, &beginInfo);

	return commandBuffer;
}
void MFVkCommandManager::setBufferCount(uint32_t count){
	m_bufferCount= count;
}
void MFVkCommandManager::endSingleTimeCommands(VkCommandBuffer *commandBuffer) {
	vkEndCommandBuffer(*commandBuffer);
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = commandBuffer;
	vkQueueSubmit(*mp_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(*mp_graphicsQueue);

	vkFreeCommandBuffers(*mp_context->getVirtualDevice(), *mp_commandPool, 1, commandBuffer);
	delete commandBuffer;
}
VkCommandBuffer* MFVkCommandManager::beginRenderCommands(){
	VkCommandBufferAllocateInfo allocInfo={};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = *mp_commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer* commandBuffer=new VkCommandBuffer();
	vkAllocateCommandBuffers(*(mp_context->getVirtualDevice()),&allocInfo,commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    vkBeginCommandBuffer(*commandBuffer, &beginInfo);

	return commandBuffer;
}



