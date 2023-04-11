/*
 * MFVkDescriptorManager.h
 *
 *  Created on: 03.09.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKSCENEMANAGER_MFVKDESCRIPTORMANAGER_H_
#define MFVULKAN_MFVKSCENEMANAGER_MFVKDESCRIPTORMANAGER_H_

class MFVkDescriptorManager;
#include "../MFVkContext/MFVkContext.h"
#include "../MFVkResources/MFVkResources.h"
#include "vulkan/vulkan.h"
#include "../MFVkObjectComponents/MFVkDescriptorSetLayout.h"
#include "../MFPredefinedObjectSetups/MFAbstractSetup.h"

class MFVkDescriptorManager {
private:
	MFVkResources
		*mp_resources;
	MFVkContext
		*mp_context;

	MFAbstractSetup
		*mp_defaultSetup;

	std::vector<VkDescriptorPool>
		*mp_vecDescriptorPool;
	VkDescriptorPool
		*mp_descriptorPool;

	std::vector<VkDescriptorSet>
		*mp_vecDescriptorSet;

	uint32_t
		m_descriptorCount=1,
		m_samplerCount=1,
		m_nextDescriptorIndex=0;

	std::vector<MFVkDescriptorSetLayout*>
		*mp_vecLayoutSetup;

	std::vector<VkDescriptorSetLayout>
		*mp_vecDescriptorSetLayout;


	uint32_t
		m_nextDescriptor=0;
	VkDescriptorPool
		*mp_objectsPool;


public:
	MFVkDescriptorManager(MFVkContext *pContext, MFVkResources *pResources);
	virtual ~MFVkDescriptorManager();

	void destroyDescriptorPool();

	bool initDescriptorManager();

	void addLayoutBinding(VkDescriptorSetLayoutBinding binding);


	bool createDescriptorSetLayout();

	bool createDescriptorPool();

	/**
	 * This method adds a layout setup. A VkDescriptorSetLayout will be added to the MFVkDescriptorSetLayout object.
	 * The returned object can be used to setup the layout.
	 * @return the layout setup object.
	 */
	MFVkDescriptorSetLayout* addLayoutSetup();
	std::vector<VkDescriptorSetLayout>* getVecLayout(){return mp_vecDescriptorSetLayout;};
	std::vector<MFVkDescriptorSetLayout*>* getVecLayoutSetup(){return mp_vecLayoutSetup;};

	void setDescriptorSetCount(uint32_t count){m_descriptorCount=count;};

	void setSamplerCount(uint32_t count);

	bool getNextDescriptorIndex(uint32_t &index);

	VkDescriptorSet* getDescriptor(uint32_t index);

};

#endif /* MFVULKAN_MFVKSCENEMANAGER_MFVKDESCRIPTORMANAGER_H_ */
