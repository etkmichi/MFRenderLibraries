/*
 * MFVkContext.cpp
 *
 *  Created on: 16.07.2019
 *      Author: michl
 */

#include "MFVkContext.h"
#include "../../MFRenderClasses/MFShaders/MFShader.h"
MFVkContext::MFVkContext(
    std::string windowTitle,
    std::string dataPath,
    std::string glslShaderCompilerPath,
    MFPrintTarget* pSubPrinter):
      MFRenderContext(pSubPrinter),
      m_dataPath(dataPath)
{
	m_applicationInfo={};
	MFShader::setCompilerPath(glslShaderCompilerPath);
	mp_defaultSetup=new MFVkSetup();
	//MFObject::getPrinterface().setTag(VULKAN_TAG,"Vulkan");
	m_vkApiVersion = VK_API_VERSION_1_1;
	m_engineVersion = VK_MAKE_VERSION(0,0,1);
	m_applicationVersion = VK_MAKE_VERSION(0,0,1);

	mp_windowContainer=new MFVkWindowContainer(windowTitle,this);
	mp_device = new MFVkDevice(this);
	mp_debugger = new MFVkDebugger(this);
	mp_deviceManager = new MFVkDeviceManager(this);
	mp_instance=nullptr;
	mp_resources=nullptr;
	mp_glfwExtensions=nullptr;
}
MFVkContext::~MFVkContext() {
	delete mp_debugger;
	vkDestroyInstance(*mp_instance, nullptr);
}


MFVkSetup* MFVkContext::getSetup(){
	return mp_defaultSetup;
}
bool MFVkContext::initContext(){
	if(m_isInit)return true;

	printInfo("MFVkContext::initContext() - Creating window for vulkan instance!");
	if(!mp_windowContainer->initWindow())
	  return false;

	if(!createInstance())
	  return false;

	printInfo("MFVkContext::initContext() - Initialicing vulkan debugger");
	if(! mp_debugger->initDebugger()){
		printErr("MFVkContext::initContext() - Couldn't init vulkan debugger! This may cause an error!");
		printInfo("MFVkContext::initContext() - if vulkan validation layers are used, don't forget to load environmen variables source /.../vulkan/setup-env.sh \nCustom setups may require #define NDEBUG for debugging");
	}

	printInfo("MFVkContext::initContext() - Initialicing glfw vulkan window");
	if(!mp_windowContainer->initVulkanWindow(mp_instance)){
		printErr("MFVkContext::initContext() - failed to init vulkan window");
		return false;
	}
	if(!mp_deviceManager->initDeviceManager(mp_instance)){
		printErr("MFVkContext::initContext() - failed to init device manager");
		return false;
	}
	mp_deviceManager->listDevices();

	/*TODO Choose device!?!*/
	mp_device->setPhysicalDevice(&(mp_deviceManager->getPhysicalDevices()->at(0)));
	if(!mp_device->initDevice(&(mp_deviceManager->getPhysicalDevices()->at(0)))){
    printErr("MFVkContext::initContext() - failed to init device!");
    return false;
	}
	m_isInit=true;
	return true;
}

GLFWwindow* MFVkContext::getWindow(){return mp_windowContainer->MFWindowContainer::getWindow();}

void MFVkContext::setWindowContainer(MFVkWindowContainer* windowContainer){mp_windowContainer=windowContainer;}

MFVkWindowContainer* MFVkContext::getWindowContainer(){
	return mp_windowContainer;
}

VkInstance MFVkContext::getInstance(){
	return *mp_instance;
}

bool MFVkContext::createInstance(){
	m_applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	m_applicationInfo.pApplicationName = "MFVulkanRenderer";
	m_applicationInfo.pEngineName = "No Engine";
	m_applicationInfo.apiVersion = VK_API_VERSION_1_1;
	m_applicationInfo.engineVersion = VK_MAKE_VERSION(1, 2, 0);
	m_applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 2, 0);/*TODO create set method.*/

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &m_applicationInfo;

	mp_glfwExtensions = mp_windowContainer->getGLFWVulkanExtensions();
	createInfo.enabledExtensionCount = mp_glfwExtensions->size();
	createInfo.ppEnabledExtensionNames = mp_glfwExtensions->data();
	printInfo("MFVkContext::createInstance - setting up validation layers!");
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(mp_debugger->getValidationLayers().size());
		createInfo.ppEnabledLayerNames = mp_debugger->getValidationLayers().data();
		createInfo.pNext =&(mp_debugger->getDebugCreateInfo());
	} else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}


	printInfo("MFVkContext::createInstance - creating instance. glfwExtensionCount: "+
	    std::to_string(mp_glfwExtensions->size()));
	mp_instance=new VkInstance();
	if (vkCreateInstance(&createInfo, nullptr, mp_instance) != VK_SUCCESS) {
		printErr("MFVkContext::createInstance() - couldn't create vulkan instance!");
		return false;
	}
	return true;
}

MFVkDevice* MFVkContext::getDevice(){
	return mp_device;
}

VkDevice* MFVkContext::getVirtualDevice(){
	return mp_device->getVirtualDevice();
}

std::vector<const char*>&  MFVkContext::getValidationLayers(){
	return mp_debugger->getValidationLayers();
}

MFVkDebugger* MFVkContext::getDebugger(){
	return mp_debugger;
}

VkSurfaceKHR* MFVkContext::getSurface(){
	return mp_windowContainer->getSurface();
}

std::vector<uint32_t>* MFVkContext::getGrapicsQueueIndices(){
	return mp_device->getGraphicsQueueIndices();
}
VkPhysicalDevice* MFVkContext::getPhysicalDevice(){
	return mp_device->getPhysicalDevice();
}


