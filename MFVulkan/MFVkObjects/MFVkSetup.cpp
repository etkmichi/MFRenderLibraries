/*
 * MFVkSetup.cpp
 *
 *  Created on: 06.08.2019
 *      Author: michl
 */

#include "MFVkSetup.h"

MFVkSetup::MFVkSetup() {
	m_graphicsQCount=1;
	m_presentationQCount=1;
	m_computeQCount=1;
	m_transferQCount=0;
	m_imageCount=2;
	m_swapChainImageCount=2;
	m_swapChainMinImageCount=2;

	deviceFeatures={};
	m_inner_maxColorAttachments=1;
	m_sceneSetupMaxObjectCount=200;
	m_surfaceFormat=VK_FORMAT_B8G8R8A8_SRGB;

	m_msaaSamples = VK_SAMPLE_COUNT_1_BIT;


	m_inner_maxColorAttachments=4;/*TODO setup while init of phyiscal device*/

		/*SwapChain*/


	m_surfaceColorSpace=VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

	/*Debugger*/
	m_debuggerMessageSeverity=
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
//			VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | //uncomment for less vulkan prints
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	m_debuggerMessageType=
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

	m_swapPresentMode=VK_PRESENT_MODE_FIFO_KHR;//VK_PRESENT_MODE_MAILBOX_KHR;

	m_windowWidth=800;
	m_windowHeight=600;
}

MFVkSetup::~MFVkSetup() {
	// TODO Auto-generated destructor stub
}

VkPhysicalDeviceFeatures* MFVkSetup::getPhysicalDeviceFeatures(){
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	return &deviceFeatures;
}
