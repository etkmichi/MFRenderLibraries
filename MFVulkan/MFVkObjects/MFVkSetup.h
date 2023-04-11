/*
 * MFVkSetup.h
 *
 *  Created on: 06.08.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKCONTEXT_MFVKSETUP_H_
#define MFVULKAN_MFVKCONTEXT_MFVKSETUP_H_

#include <MFObjects/MFObject.h>
#include <vulkan/vulkan.h>

/**
 * This class contains predefined setups for the vulkan render library.
 */
class MFVkSetup: public MFObject {
private:

public:
	/*Other queues neccessary?*/
	uint32_t
		m_graphicsQCount,
		m_presentationQCount,
		m_computeQCount,
		m_transferQCount,
		m_imageCount,
		m_swapChainImageCount,
		m_swapChainMinImageCount;

	uint32_t
		m_sceneSetupMaxObjectCount;
	uint32_t
		m_windowWidth,
		m_windowHeight;

	VkSampleCountFlagBits
		m_msaaSamples = VK_SAMPLE_COUNT_8_BIT;

	uint32_t
		m_inner_maxColorAttachments;/*TODO setup while init of phyiscal device*/

	/*SwapChain*/
	VkFormat
		m_surfaceFormat;
	VkColorSpaceKHR
		m_surfaceColorSpace=VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

	/*Debugger*/
	VkDebugUtilsMessageSeverityFlagsEXT m_debuggerMessageSeverity=
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			//VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	VkDebugUtilsMessageTypeFlagsEXT m_debuggerMessageType=
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

	VkPresentModeKHR
		m_swapPresentMode;

	VkPhysicalDeviceFeatures
		deviceFeatures;
	MFVkSetup();
	virtual ~MFVkSetup();
	VkPhysicalDeviceFeatures* getPhysicalDeviceFeatures();
};

#endif /* MFVULKAN_MFVKCONTEXT_MFVKSETUP_H_ */
