/*
 * MFVkDeviceManager.h
 *
 *  Created on: 05.08.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKCONTEXT_MFVKDEVICEMANAGER_H_
#define MFVULKAN_MFVKCONTEXT_MFVKDEVICEMANAGER_H_

#include <MFObjects/MFObject.h>
#include "vulkan/vulkan.h"
#include <vector>
#include <iterator>
#include "../../MFRenderContext.h"
/**
 * The vulkan device manager provides support functionality for all devices which support vulkan.
 */
class MFVkDeviceManager : public MFObject {
private:
  MFRenderContext
  *mp_context;

	unsigned int m_deviceCount;
	VkInstance* mp_instance;
	std::vector<VkPhysicalDevice>* mp_vecPhysicalDevices;
	std::vector<VkPhysicalDeviceProperties>* mp_vecPhysicalDeviceProperties;
public:
	MFVkDeviceManager(MFRenderContext* pContext);
	MFVkDeviceManager(VkInstance* pInstance,MFRenderContext* pContext);
	virtual ~MFVkDeviceManager();
	bool initDeviceManager(VkInstance* pInstance);
	void listDevices();
	std::vector<VkPhysicalDevice>* getPhysicalDevices();
};

#endif /* MFVULKAN_MFVKCONTEXT_MFVKDEVICEMANAGER_H_ */
