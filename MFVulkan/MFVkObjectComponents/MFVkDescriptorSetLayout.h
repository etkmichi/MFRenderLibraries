/*
 * MFVkDescriptorSetLayout.h
 *
 *  Created on: 13.08.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKCONTEXT_MFVKRESOURCES_MFVKDESCRIPTORSETLAYOUT_H_
#define MFVULKAN_MFVKCONTEXT_MFVKRESOURCES_MFVKDESCRIPTORSETLAYOUT_H_

class MFVkDescriptorSetLayout;
#include <vulkan/vulkan.h>
#include <MFObjects/MFObject.h>
#include "../MFVkResources/MFVkResources.h"
#include "../MFVkContext/MFVkContext.h"
#include <vector>

/**
 * This class can be used to create a layout for descriptors.
 * The layout must represent global variables in shaders.
 */
class MFVkDescriptorSetLayout {
private:
	MFVkContext
		*mp_context;

	MFVkResources
		*mp_resources;

	std::vector<VkDescriptorSetLayoutBinding>
		*mp_vecDescriptorBinding;

	VkDescriptorSetLayoutBinding
		*mp_descriptorUBO,
		*mp_descriptorSampler;

	VkDescriptorSetLayout
		*mp_descriptorSetLayout;
public:
	MFVkDescriptorSetLayout(MFVkContext* pContext, MFVkResources* pResources, VkDescriptorSetLayout* descriptorSetLayout);
	virtual ~MFVkDescriptorSetLayout();

	/*TODO add functions for other descriptor sets*/
	uint32_t addDescriptorSet(VkDescriptorSetLayoutBinding &descriptor);

	/**
	 * Adds a descriptor layout binding for an UBO. An UBO on shader side represents a global variable (for example a model rotation/translation which must be used for every vertex).
	 * An UBO on vulkan side will be represented by a descriptor which in turn represents an UBO on client side. The ubo on client side can be a 4x4 matrix which may hold some rotation/
	 * translation data. To use the client data, every render loop must add commands for binding vertices, indices and descriptor to make the data usable in the shader.
	 * @param binding
	 * @return
	 */
	uint32_t addUBO(uint32_t binding);
	uint32_t addSampler(uint32_t binding);
	bool createDescriptorSetLayout();
	std::vector<VkDescriptorSetLayoutBinding>* getDescriptors();

	/**
	 * Returns a pointer to the latest added ubo descriptor.
	 * @return
	 */
	VkDescriptorSetLayoutBinding* getUboDescriptor();

	/**
	 *
	 * @return a pointer to the latest added sampler desciptor.
	 */
	VkDescriptorSetLayoutBinding* getSamplerDescriptor();
	VkDescriptorSetLayoutBinding* getDescriptor(uint32_t position);
	//TODO addLayout function
	//TODO getLayoutCount getLayouts -> fix pipeline compatiblity!
	VkDescriptorSetLayout* getDescriptorSetLayout(){return mp_descriptorSetLayout;};
};

#endif /* MFVULKAN_MFVKCONTEXT_MFVKRESOURCES_MFVKDESCRIPTORSETLAYOUT_H_ */
