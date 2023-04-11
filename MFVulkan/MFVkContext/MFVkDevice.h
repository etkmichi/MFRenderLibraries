/*
 * MFVkDevice.h
 *
 *  Created on: 18.07.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKCONTEXT_MFVKDEVICE_H_
#define MFVULKAN_MFVKCONTEXT_MFVKDEVICE_H_

class MFVkDevice;
#include "vulkan/vulkan.h"
#include "MFObjects/MFObject.h"
#include "MFVkDebugger.h"
#include "MFVkContext.h"
class MFVkContext;
#include <vector>
#include <set>
#include <map>
/**
 * This class wraps arround a physical device which supports vulkan. In the initialization it will retrieve all important information from the vulkan api.
 * The fields of this class contain the resources of the vulkan device.
 */
class MFVkDevice{
private:
	VkPhysicalDevice
		*mp_physicalDevice;

	VkPhysicalDeviceFeatures
		*mp_physicalDeviceFeatures;

	VkPhysicalDeviceMemoryProperties
		*mp_memoryProperties;

	VkPhysicalDeviceProperties
		*mp_physicalDeviceProperties;

	float
		m_queuePriority = 1.0f;
	std::vector<VkDeviceQueueCreateInfo>
		m_queueCreateInfos;

	VkDeviceCreateInfo
		*mp_deviceCreateInfo;

	VkDevice
		*mp_virtualDevice;

	std::vector<VkQueueFamilyProperties>
		*mp_vecQFamilyProperties;

	VkQueue
		*mp_graphicsQueue,
		*mp_presentQueue,
		*mp_computeQueue;

	std::vector<const char*>
		m_deviceExtensions;

	std::vector<uint32_t>
		*mp_vecGraphicsQueueIndices,
		*mp_vecPresentationQueueIndices,
		*mp_vecSparseBindingQueueIndices,
		*mp_vecComputeQueueIndices,
		*mp_vecTransferQueueIndices,
		*mp_vecProtectedQueueIndices,
		*mp_vecMaxEnumQueueIndices;

	std::set<uint32_t>
		*mp_setUsedQueueIndices;

	MFVkContext
		*mp_context;
	void init();

public:
	/**
	 *
	 * @param pContext - needed for additional resources during initialization!
	 */
	MFVkDevice(MFVkContext* pContext);

	/**
	 *
	 * @param pContext - needed for additional resources during initialization!
	 * @param pDevice - the vulkan device which should be wrapped
	 */
	MFVkDevice(MFVkContext* pContext,VkPhysicalDevice* pDevice);
	virtual ~MFVkDevice();

	/**
	 * Initialices the logical device and all surrounding informations.
	 * Important pre initialications:
	 * - surface must be initialiced before
	 * - vulkan instance must be initialiced before
	 * @return true if all initialization calls were successfull.
	 */
	bool initDevice(VkPhysicalDevice* device);
	bool initVirtualDevice();
	bool initQueueFamilies();

	VkQueue* getTransferQueue();
	VkQueue* getCalculationQueue(){return mp_computeQueue;};
	VkQueue* getPresentationQueue(){return mp_presentQueue;};
	VkQueue* getGraphicsQueue(){return mp_graphicsQueue;};

	std::vector<uint32_t>* getGraphicsQueueIndices(){return mp_vecGraphicsQueueIndices;};
	std::vector<uint32_t>* getPresentationQueueIndices(){return mp_vecPresentationQueueIndices;};
	std::vector<uint32_t>* getComputeQueueIndices(){return mp_vecComputeQueueIndices;};

	VkFormatProperties getFormatProperties(VkFormat format);

	bool checkFormatPropertyOptimalTiling(VkFormat format,VkFormatFeatureFlags enumFlags);


	VkDevice* getVirtualDevice();
	void setVirtualDevice(VkDevice* pVDevice);

	VkPhysicalDevice* getPhysicalDevice();
	void setPhysicalDevice(VkPhysicalDevice* pDevice);

	std::set<uint32_t>* getUsedQueueIndices();
	/**
	 * Use this get funciton to setup the physical device features before creating a virtual device.
	 * @return the pointer to VkPhysicalDeviceFeatures structure for custom setup.
	 */
	VkPhysicalDeviceFeatures* getPhysicalDeviceFeatures();

	VkPhysicalDeviceMemoryProperties* getMemoryProperties();

	/**
	 *
	 * @param memoryIndex output data
	 * @param memRequirements in data pointer for search operation
	 * @param flags flags for search operation
	 * @return
	 */
	bool getMemoryIndex(
			uint32_t& memoryIndex,
			VkMemoryRequirements* memRequirements,
			VkMemoryPropertyFlags flags);

	bool findSupportedFormat(
			const std::vector<VkFormat>& candidates,
			VkImageTiling tiling,
			VkFormatFeatureFlags features,
			VkFormat* pFormat);
	/**
	 * Used while filling the create info of the virtual devices creation. Uses the MFVkSetup data to fill the structure.
	 * @return a vector with queue creation info.
	 */
	std::vector<VkDeviceQueueCreateInfo> createQueueInfos();

};

#endif /* MFVULKAN_MFVKCONTEXT_MFVKDEVICE_H_ */
