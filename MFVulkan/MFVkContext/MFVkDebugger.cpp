/*
 * MFVkDebugger.cpp
 *
 *  Created on: 02.08.2019
 *      Author: michl
 */

#include "MFVkDebugger.h"

MFVkDebugger::MFVkDebugger(MFVkContext* context):
m_debugMessenger()
{
  mp_context=context;
	m_debugCreateInfo={};
	if (enableValidationLayers && !checkValidationLayerSupport()) {
		mp_context->printErr("validation layers requested, but not available!");
	}

	m_debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	m_debugCreateInfo.pfnUserCallback = debugCallback;
	m_debugCreateInfo.pUserData=mp_context;

	mp_context=context;
}

MFVkDebugger::~MFVkDebugger() {
	printInfo("~MFVkDebugger()");
	if (enableValidationLayers && mp_context!=nullptr) {
		std::string info="destroying vk debug utils messenger ";
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
				vkGetInstanceProcAddr(mp_context->getInstance(), "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			info+="with vkDestroyDebugUtilsMessengerEXT";
			func(mp_context->getInstance(), m_debugMessenger, nullptr);
		}else{
			info+="failed, vkDestroyDebugUtilsMessengerEXT not found!";
			printErr(info);
			return;
		}
		printInfo(info);
	}else{
		printInfo("vk debugger/debugging layers were disabled!");
	}
}

bool MFVkDebugger::initDebugger(){
	bool ret=initMessenger();
	return ret;
}

bool MFVkDebugger::checkValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (std::strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

bool MFVkDebugger::initMessenger(){
	m_debugCreateInfo.messageSeverity = mp_context->getSetup()->m_debuggerMessageSeverity;
	m_debugCreateInfo.messageType = mp_context->getSetup()->m_debuggerMessageType;
	if (!enableValidationLayers) return true;
	if(mp_context==nullptr || mp_context->getInstance()==nullptr){
		printErr("MFVkDebugger::initMessenger() - mp_context==nullptr || mp_context->getInstance()==nullptr");
		return false;
	}

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(mp_context->getInstance(), "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(mp_context->getInstance(), &m_debugCreateInfo, nullptr, &m_debugMessenger);
	} else {
		printErr("MFVkDebugger::initMessenger() Debug extension not present!!!");
		return false;
	}
	printInfo(" MFVkDebugger::initMessenger() - done!");
	return true;
}

/*static*/
VKAPI_ATTR VkBool32 VKAPI_CALL MFVkDebugger::debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {
  MFVkContext *pContext=static_cast<MFVkContext*>(pUserData);
  pContext->printWarning("validation layer: "+std::string(pCallbackData->pMessage));
	return VK_FALSE;
}
