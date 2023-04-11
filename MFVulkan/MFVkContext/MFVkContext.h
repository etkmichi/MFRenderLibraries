/*
 * MFVkContext.h
 *
 *  Created on: 16.07.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKCONTEXT_H_
#define MFVULKAN_MFVKCONTEXT_H_
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

//#define TINYOBJLOADER_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION //Not in header files!
class MFVkContext;

#include <vector>
#include "../MFVkObjects/MFVkStructs.h"
#include "MFVkWindowContainer.h"
#include "MFObjects/MFObject.h"
#include "MFVkDevice.h"
#include "../MFVkResources/MFVkResources.h"
#include "MFVkDebugger.h"
#include "MFVkDeviceManager.h"
#include "../MFVkObjects/MFVkSetup.h"
#include "../../MFRenderContext.h"
/**
 * Contains the VkDevice physical/logical and other base resources.
 */
class MFVkContext : public MFRenderContext{
private:
	MFVkSetup*
		mp_defaultSetup;

	VkInstance*
		mp_instance;

	VkApplicationInfo
		m_applicationInfo;

	MFVkDevice*
		mp_device;

	MFVkResources*
		mp_resources;

	MFVkDebugger*
		mp_debugger;

	MFVkWindowContainer*
		mp_windowContainer;

	MFVkDeviceManager*
		mp_deviceManager;

	std::vector<const char*>
		*mp_glfwExtensions;
	std::string
		m_dataPath;

	uint32_t /*setter!*/
		m_vkApiVersion,
		m_engineVersion,
		m_applicationVersion;

	bool m_isInit=false;


public:
	/**
	 *
	 * @param windowTitle
	 * @param dataPath - path to store / load data for the render engine
	 */
	MFVkContext(
	    std::string windowTitle,
	    std::string dataPath,
	    std::string glslShaderCompilerPath,
	    MFPrintTarget* pSubPrinter=nullptr);
	virtual ~MFVkContext();
	MFVkDevice* getDevice();
	VkDevice*  getVirtualDevice();
	VkPhysicalDevice* getPhysicalDevice();
	VkSurfaceKHR* getSurface();
	MFVkResources* getResources(){return mp_resources;};
	void setResources(MFVkResources* pResources){mp_resources=pResources;};
	std::vector<uint32_t>* getGrapicsQueueIndices();
	bool getGraphicsQueue(uint32_t &queueIndex);
	GLFWwindow* getWindow();
	bool initContext();
	bool checkValidationLayerSupport();

	MFVkSetup* getSetup();
	std::string getDataPath(){return m_dataPath;};
	void setWindowContainer(MFVkWindowContainer* windowContainer);

	MFVkWindowContainer* getWindowContainer();

	VkInstance getInstance();

	std::vector<const char*>& getValidationLayers();
	MFVkDebugger* getDebugger();

private:

	/**
	 * Vulkan instance creation. Must be destoyed in Constructor!
	 */
	bool createInstance();

};

#endif /* MFVULKAN_MFVKCONTEXT_H_ */
