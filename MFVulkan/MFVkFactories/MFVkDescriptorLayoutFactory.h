/*
 * MFVkDescriptorLayoutCreator.h
 *
 *  Created on: 18.09.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKFACTORIES_MFVKDESCRIPTORLAYOUTFACTORY_H_
#define MFVULKAN_MFVKFACTORIES_MFVKDESCRIPTORLAYOUTFACTORY_H_
#include "vulkan/vulkan.h"
#include "../MFVkObjects/MFVkStructs.h"
class MFVkDescriptorLayoutFactory {
private:
	DescriptorLayoutSetup
		*mp_defaultSetup;
	std::vector<VkDescriptorSetLayoutBinding>
		m_vecDefaultBinding;


	void initDefaultSetup();
public:
	MFVkDescriptorLayoutFactory();
	virtual ~MFVkDescriptorLayoutFactory();

	bool createDefaultLayout(VkDevice device, VkDescriptorSetLayout* pLayout);

	DescriptorLayoutSetup* getDefaultLayout();
};

#endif /* MFVULKAN_MFVKFACTORIES_MFVKDESCRIPTORLAYOUTFACTORY_H_ */
