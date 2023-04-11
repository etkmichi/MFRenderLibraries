/*
 * MFVkDebugger.h
 *
 *  Created on: 02.08.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKCONTEXT_MFVKDEBUGGER_H_
#define MFVULKAN_MFVKCONTEXT_MFVKDEBUGGER_H_

class MFVkDebugger;

#include <MFObjects/MFObject.h>
#include <vulkan/vulkan.h>
#include "MFVkContext.h"

#ifdef NDEBUG
const bool enableValidationLayers = true;
#else
const bool enableValidationLayers = true;
#endif

class MFVkDebugger: public MFObject {
private:
	MFVkContext*
		mp_context;

	VkDebugUtilsMessengerCreateInfoEXT
		m_debugCreateInfo;

	VkDebugUtilsMessengerEXT
		m_debugMessenger;

	std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
	};//"VK_LAYER_KHRONOS_validation" VK_LAYER_LUNARG_standard_validation

public:
	MFVkDebugger(MFVkContext* context);
	virtual ~MFVkDebugger();
	VkDebugUtilsMessengerCreateInfoEXT& getDebugCreateInfo(){return m_debugCreateInfo;};
	std::vector<const char*>& getValidationLayers(){return validationLayers;};
	bool initDebugger();
	bool initMessenger();
	bool checkValidationLayerSupport();
	static bool isDebuggingEnabled(){return enableValidationLayers;};

	/**
	 * Static callback for vulkan api. Will be given to vulkan api at setup time.
	 * @param messageSeverity
	 * @param messageType
	 * @param pCallbackData
	 * @param pUserData
	 * @return
	 */
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);
};

#endif /* MFVULKAN_MFVKCONTEXT_MFVKDEBUGGER_H_ */
