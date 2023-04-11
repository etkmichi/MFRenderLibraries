/*
 * MFVkDevice.cpp
 *
 *  Created on: 18.07.2019
 *      Author: michl
 */

#include "MFVkDevice.h"

MFVkDevice::MFVkDevice(MFVkContext* pContext) {
	mp_physicalDevice=nullptr;/*Handle can be found by MFVkDeviceManager and set by member function of this class.*/
	mp_context=pContext;
	init();
}

MFVkDevice::MFVkDevice(MFVkContext* pContext,VkPhysicalDevice* pDevice){
	mp_physicalDevice=pDevice;
	mp_context=pContext;
	init();
}

MFVkDevice::~MFVkDevice() {
	if(mp_vecQFamilyProperties!=nullptr){
		delete mp_vecQFamilyProperties;
	}
	delete mp_vecGraphicsQueueIndices;
	delete mp_vecPresentationQueueIndices;
	delete mp_vecComputeQueueIndices;
	delete mp_vecTransferQueueIndices;
	delete mp_vecProtectedQueueIndices;
	delete mp_vecMaxEnumQueueIndices;
	delete mp_vecSparseBindingQueueIndices;
	delete mp_setUsedQueueIndices;
	delete mp_graphicsQueue;
	delete mp_presentQueue;
	delete mp_computeQueue;


}


void MFVkDevice::init(){
	m_queueCreateInfos = {};
	mp_virtualDevice=new VkDevice();
	mp_memoryProperties = new VkPhysicalDeviceMemoryProperties();
	mp_physicalDeviceFeatures = new VkPhysicalDeviceFeatures();
	mp_vecQFamilyProperties = nullptr;
	mp_deviceCreateInfo = new VkDeviceCreateInfo();
	mp_vecGraphicsQueueIndices = new std::vector<uint32_t>();
	mp_vecPresentationQueueIndices = new std::vector<uint32_t>();
	mp_vecComputeQueueIndices = new std::vector<uint32_t>();
	mp_vecTransferQueueIndices = new std::vector<uint32_t>();
	mp_vecProtectedQueueIndices = new std::vector<uint32_t>();
	mp_vecMaxEnumQueueIndices = new std::vector<uint32_t>();
	mp_vecSparseBindingQueueIndices = new std::vector<uint32_t>();
	mp_setUsedQueueIndices = new std::set<uint32_t>();
	m_deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_EXT_DEPTH_RANGE_UNRESTRICTED_EXTENSION_NAME
	};
	mp_graphicsQueue = new VkQueue();
	mp_presentQueue = new VkQueue();
	mp_computeQueue = new VkQueue();
}

bool MFVkDevice::findSupportedFormat(
		const std::vector<VkFormat>& candidates,
		VkImageTiling tiling,
		VkFormatFeatureFlags features,
		VkFormat* pFormat)
{
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(*mp_physicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			*pFormat=format;
			return true;
		} else if(tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			*pFormat=format;
			return true;
		}
	}

	mp_context->printErr("MFVkDevice::findSupportedFormat - no supported format found!");
	return false;
}

bool MFVkDevice::checkFormatPropertyOptimalTiling(VkFormat format,VkFormatFeatureFlags enumFlags){
	VkFormatProperties formatProperties={};
	vkGetPhysicalDeviceFormatProperties(*mp_physicalDevice, format, &formatProperties);

	if (!(formatProperties.optimalTilingFeatures & enumFlags)) {
		mp_context->printErr("No support for given format feature!");
		return false;
	}
	return true;
}

VkFormatProperties MFVkDevice::getFormatProperties(VkFormat format){
	VkFormatProperties formatProperties={};
	vkGetPhysicalDeviceFormatProperties(*mp_physicalDevice, format, &formatProperties);
	return formatProperties;
}

bool MFVkDevice::initDevice(VkPhysicalDevice* device){
	mp_context->printInfo("MFVkDevice::initDevice(VkPhysicalDevice* device)");
	mp_physicalDevice=device;
	if(mp_physicalDevice==nullptr){
		mp_context->printErr("MFVkDevice::initDevice(VkPhysicalDevice* device) - No physical device for device initialisdication!");
		return false;
	}
	if(!initQueueFamilies()){
		mp_context->printErr("MFVkDevice::initDevice(VkPhysicalDevice* device) - Failed to init queue families!");
		return false;
	}

	vkGetPhysicalDeviceFeatures(*mp_physicalDevice, mp_physicalDeviceFeatures);
	vkGetPhysicalDeviceMemoryProperties(*mp_physicalDevice, mp_memoryProperties);

	if(!initVirtualDevice()){
		mp_context->printErr("Failed to init queue families!");
		return false;
	}

	return true;
}

VkPhysicalDeviceMemoryProperties* MFVkDevice::getMemoryProperties(){
	return mp_memoryProperties;
}

bool MFVkDevice::getMemoryIndex(uint32_t& memoryIndex,VkMemoryRequirements* memRequirements,VkMemoryPropertyFlags flags){
	try{
		for (uint32_t i = 0; i < mp_memoryProperties->memoryTypeCount; i++) {
			if ((memRequirements->memoryTypeBits & (1 << i)) && (mp_memoryProperties->memoryTypes[i].propertyFlags & flags) == flags) {
				memoryIndex=i;
				return true;
			}
		}
		mp_context->printWarning("MFVkDevice::getMemoryIndex -No memory matches property flags!");
	}catch(std::exception &e){
		mp_context->printErr("MFVkDevice::getMemoryIndex - No valid memory found! "+std::string(e.what()));
	}
	mp_context->printWarning("MFVkDevice::getMemoryIndex - No memory matches the requirements!");
	return false;
}

bool MFVkDevice::initQueueFamilies(){
	if(mp_physicalDevice==nullptr){
		mp_context->printErr("No physical device set! Can't retrive queue families!");
		return false;
	}
	uint32_t qFamCount=0;
	vkGetPhysicalDeviceQueueFamilyProperties(*mp_physicalDevice, &qFamCount, nullptr);

	if(qFamCount==0){
		mp_context->printErr("No queue family found!");
		return false;
	}
	if(mp_vecQFamilyProperties!=nullptr){
		delete mp_vecQFamilyProperties;
	}

	mp_vecQFamilyProperties=new std::vector<VkQueueFamilyProperties>(qFamCount);
	vkGetPhysicalDeviceQueueFamilyProperties(*mp_physicalDevice, &qFamCount, mp_vecQFamilyProperties->data());
	mp_context->printInfo("MFVkDevice::initQueueFamilies() - VkQueueFamilyProperties count="+std::to_string(qFamCount));

	int counter=0;
	for(VkQueueFamilyProperties queueFamilyProperties: (*mp_vecQFamilyProperties)){
		if(queueFamilyProperties.queueCount==0){
			mp_context->printInfo("Queue family count = 0!");
			continue;
		}
		if(queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT){
			mp_context->printInfo("Queue with VK_QUEUE_GRAPHICS_BIT found. queue index="+std::to_string(counter));
			mp_vecGraphicsQueueIndices->push_back(counter);
		}
		if(queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT){
			mp_context->printInfo("Queue with VK_QUEUE_COMPUTE_BIT found. queue index="+std::to_string(counter));
			mp_vecComputeQueueIndices->push_back(counter);
		}
		if(queueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT){
			mp_context->printInfo("Queue with VK_QUEUE_TRANSFER_BIT found. queue index="+std::to_string(counter));
			mp_vecTransferQueueIndices->push_back(counter);
		}
		if(queueFamilyProperties.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT){
			mp_context->printInfo("Queue with VK_QUEUE_SPARSE_BINDING_BIT found. queue index="+std::to_string(counter));
			mp_vecSparseBindingQueueIndices->push_back(counter);
		}
		if(queueFamilyProperties.queueFlags & VK_QUEUE_PROTECTED_BIT){
			mp_context->printInfo("Queue with VK_QUEUE_PROTECTED_BIT found. queue index="+std::to_string(counter));
			mp_vecProtectedQueueIndices->push_back(counter);
		}
		if(queueFamilyProperties.queueFlags & VK_QUEUE_FLAG_BITS_MAX_ENUM){
			mp_context->printInfo("Queue with VK_QUEUE_FLAG_BITS_MAX_ENUM found. queue index="+std::to_string(counter));
			mp_vecMaxEnumQueueIndices->push_back(counter);
		}
		/*TODO mir, wayland, win32, xcb xlib...*/
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(*mp_physicalDevice, counter, *(mp_context->getSurface()), &presentSupport);
		if (presentSupport) {
			mp_context->printInfo("Queue is KHR presentation compatible. queue index="+std::to_string(counter));
			mp_vecPresentationQueueIndices->push_back(counter);
		}
		/*TODO other surfaces supports!*/
		counter++;
	}
	return true;
}

std::vector<VkDeviceQueueCreateInfo> MFVkDevice::createQueueInfos(){

	std::set<uint32_t> uniqueQueueFamilies = {};
	for(uint32_t i=0;i<mp_context->getSetup()->m_graphicsQCount;i++){
		if(mp_vecGraphicsQueueIndices->size()==0){
			mp_context->printErr("MFVkDevice::createQueueInfos() - "
					"Can't add graphics queue create info in device creation! No graphics queue index available!");
			break;
		}
		int position = i%(mp_vecGraphicsQueueIndices->size());
		uniqueQueueFamilies.insert(mp_vecGraphicsQueueIndices->at(position));
	}
	for(uint32_t i=0;i<mp_context->getSetup()->m_presentationQCount;i++){
		if(mp_vecPresentationQueueIndices->size()==0){
			mp_context->printErr("MFVkDevice::createQueueInfos() - "
					"Can't add presentation queue create info in device creation! No presentation queue index available!");
			break;
		}
		int position = i%(mp_vecPresentationQueueIndices->size());
		uniqueQueueFamilies.insert(mp_vecPresentationQueueIndices->at(position));
	}
	for(uint32_t i=0;i<mp_context->getSetup()->m_computeQCount;i++){
		if(mp_vecComputeQueueIndices->size()==0){
			mp_context->printErr("MFVkDevice::createQueueInfos() - "
					"Can't add compute queue create info in device creation! No compute queue index available!");
			break;
		}
		int position = i%(mp_vecComputeQueueIndices->size());
		uniqueQueueFamilies.insert(mp_vecComputeQueueIndices->at(position));
	}
	for(uint32_t i=0;i<mp_context->getSetup()->m_transferQCount;i++){
		if(mp_vecTransferQueueIndices->size()==0){
			mp_context->printErr("MFVkDevice::createQueueInfos() - "
					"Can't add transfer queue create info in device creation! No transfer queue index available!");
			break;
		}
		int position = i%(mp_vecTransferQueueIndices->size());
		uniqueQueueFamilies.insert(mp_vecTransferQueueIndices->at(position));
	}

	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &m_queuePriority;
		queueCreateInfo.flags=0;
		m_queueCreateInfos.push_back(queueCreateInfo);
		mp_context->printInfo("MFVkDevice::createQueueInfos() - added queue create info with index: "+
				std::to_string(queueFamily));
	}
	return m_queueCreateInfos;
}

bool MFVkDevice::initVirtualDevice(){
	/**/
	createQueueInfos();
	//mp_physicalDeviceFeatures = mp_context->getSetup()->getPhysicalDeviceFeatures();
//	mp_physicalDeviceFeatures->
	VkDeviceCreateInfo createInfo = {};
	VkPhysicalDeviceFeatures deviceFeatures = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(m_queueCreateInfos.size());
	createInfo.pQueueCreateInfos = m_queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;//TODO ??
	createInfo.enabledExtensionCount = static_cast<uint32_t>(m_deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = m_deviceExtensions.data();
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(mp_context->getValidationLayers().size());
		createInfo.ppEnabledLayerNames = mp_context->getValidationLayers().data();
	} else {
		createInfo.enabledLayerCount = 0;
	}
	mp_context->printInfo("MFVkDevice::initVirtualDevice() - added queue create info with index:\n "+
			std::to_string(m_queueCreateInfos.size())+"/"+
					std::to_string(m_queueCreateInfos.at(0).queueCount)+"/"+
					std::to_string(*m_queueCreateInfos.at(0).pQueuePriorities)+"/"+
					std::to_string(m_queueCreateInfos.at(0).queueFamilyIndex));
	if (vkCreateDevice(*mp_physicalDevice, &createInfo, nullptr, mp_virtualDevice) != VK_SUCCESS) {
		mp_context->printErr("MFVkDevice::initVirtualDevice() - vulkan device creation failed!");
		return false;
	}


	vkGetDeviceQueue(*mp_virtualDevice, mp_vecGraphicsQueueIndices->at(0),0,mp_graphicsQueue);
	mp_setUsedQueueIndices->insert(mp_vecGraphicsQueueIndices->at(0));

	vkGetDeviceQueue(*mp_virtualDevice, mp_vecPresentationQueueIndices->at(0),0,mp_presentQueue);
	mp_setUsedQueueIndices->insert(mp_vecPresentationQueueIndices->at(0));

	//todo other queues... Really=? -> according to setup...
	return true;
}

std::set<uint32_t>* MFVkDevice::getUsedQueueIndices(){
	return mp_setUsedQueueIndices;
}

void MFVkDevice::setVirtualDevice(VkDevice* pVDevice){
	if(mp_virtualDevice!=pVDevice && mp_virtualDevice!=nullptr){
		delete mp_virtualDevice;
	}
	mp_virtualDevice=pVDevice;
}

VkDevice* MFVkDevice::getVirtualDevice(){
	return mp_virtualDevice;
}

VkPhysicalDevice* MFVkDevice::getPhysicalDevice(){
	return mp_physicalDevice;
}

void MFVkDevice::setPhysicalDevice(VkPhysicalDevice* pDevice){
	if(mp_physicalDevice!=pDevice && mp_physicalDevice!=nullptr){
		delete mp_physicalDevice;
	}
	mp_physicalDevice=pDevice;
}

