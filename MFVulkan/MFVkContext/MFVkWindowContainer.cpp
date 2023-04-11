/*
 * MFVkWindowContainer.cpp
 *
 *  Created on: 05.08.2019
 *      Author: michl
 */

#include "MFVkWindowContainer.h"

MFVkWindowContainer::MFVkWindowContainer(
    std::string windowTitle,
    MFRenderContext *pContext):
MFWindowContainer(windowTitle,pContext) {
  mp_context=pContext;
  if(mp_context==nullptr){
    mp_context=MFRenderContext::smp_staticContext;
  }
	mp_vkSurface=new VkSurfaceKHR();
	mp_vkInstance=new VkInstance();
	mp_extensions=nullptr;
}

MFVkWindowContainer::~MFVkWindowContainer() {
	mp_context->printInfo("Destroying MFVkWindowContainer!");
	if(mp_vkInstance!=nullptr && mp_vkSurface!=nullptr){
		mp_context->printInfo("Destroying glfw surface khr of the created vulkan surface!");
		vkDestroySurfaceKHR(*mp_vkInstance, *mp_vkSurface, nullptr);
	}
	if(mp_extensions!=nullptr)
		delete mp_extensions;
	delete mp_vkInstance;
	delete mp_vkSurface;
}

bool MFVkWindowContainer::createVulkanSurface(VkInstance* instance){
	mp_context->printInfo("MFVkWindowContainer::createVulkanSurface(VkInstance* instance)");
	VkResult res=glfwCreateWindowSurface(*instance, getWindow(), nullptr, mp_vkSurface);
	if (res != VK_SUCCESS) {
		mp_context->printErr("MFVkWindowContainer::createVulkanSurface(VkInstance* instance) - "
				"failed to create window vulkan surface!"+std::to_string(res));
		return false;
	}
	return true;
}

std::vector<const char*>* MFVkWindowContainer::getGLFWVulkanExtensions(){
	uint32_t extCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&extCount);

	if(mp_extensions!=nullptr)
		delete mp_extensions;

	mp_extensions=new std::vector<const char*>(glfwExtensions, glfwExtensions + extCount);

	if(enableValidationLayers){
		mp_context->printInfo("MFVkWindowContainer::getGLFWVulkanExtensions() - validation layer enabled!");
		mp_extensions->push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return mp_extensions;
}

bool MFVkWindowContainer::initVulkanWindow(VkInstance* instance){
	mp_vkInstance=instance;
	if(!createVulkanSurface(mp_vkInstance)){
		mp_context->printErr(
		    "MFVkWindowContainer::initVulkanWindow(VkInstance* instance) - failed!");
		return false;
	}
	getFramebufferSize(&m_frameBufferWidth, &m_frameBufferHeight);
	mp_context->printInfo(
	    "MFVkWindowContainer::initVulkanWindow framebuffer width/height: "+
			std::to_string(m_frameBufferWidth)+"/"+std::to_string(m_frameBufferHeight));
	return true;
}

VkSurfaceKHR* MFVkWindowContainer::getSurface(){
	return mp_vkSurface;
}

void MFVkWindowContainer::getFramebufferSize(int *width,int *height){
	if(width==nullptr || height==nullptr)
		return;
	glfwGetFramebufferSize(getWindow(), width, height);
}

VkExtent2D MFVkWindowContainer::getFramebufferExtent(){
	VkExtent2D extent;
	int width=0,height=0;
	glfwGetFramebufferSize(getWindow(), &width, &height);
	extent.width=width;
	extent.height=height;
	return extent;
}
