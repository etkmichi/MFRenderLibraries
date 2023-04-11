/*
 * MFVkWindowContainer.h
 *
 *  Created on: 05.08.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKCONTEXT_MFVKWINDOWCONTAINER_H_
#define MFVULKAN_MFVKCONTEXT_MFVKWINDOWCONTAINER_H_

class MFVkWindowContainer;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include "../../MFWindow/MFWindowContainer.h"
#include "MFVkDebugger.h"

class MFVkWindowContainer : public MFWindowContainer {
private:
	VkSurfaceKHR*
		mp_vkSurface;

	VkInstance*
		mp_vkInstance;

	int
		m_frameBufferWidth=0,
		m_frameBufferHeight=0;

	std::vector<const char*>
		*mp_extensions;

  MFRenderContext
  *mp_context;
public:
	MFVkWindowContainer(std::string windowName,MFRenderContext *pContext);
	~MFVkWindowContainer();

	/**
	 * Initialices a glfw window with vulkan context. Calls the parent method initWindow()!
	 * @param instance
	 */
	bool initVulkanWindow(VkInstance* instance);

	std::vector<const char*>* getGLFWVulkanExtensions();

	bool createVulkanSurface(VkInstance* instance);

	VkSurfaceKHR* getSurface();

	void getFramebufferSize(int *width,int *height);
	int getFrambufferWidth(){return m_frameBufferWidth;};
	int getFramebufferHeight(){return m_frameBufferWidth;};

	VkExtent2D getFramebufferExtent();

};

#endif /* MFVULKAN_MFVKCONTEXT_MFVKWINDOWCONTAINER_H_ */
