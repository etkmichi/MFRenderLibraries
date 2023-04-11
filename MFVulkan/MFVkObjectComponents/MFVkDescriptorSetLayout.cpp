/*
 * MFVkDescriptorSetLayout.cpp
 *
 *  Created on: 13.08.2019
 *      Author: michl
 */

#include "MFVkDescriptorSetLayout.h"

MFVkDescriptorSetLayout::MFVkDescriptorSetLayout(MFVkContext* pContext, MFVkResources* pResources, VkDescriptorSetLayout* descriptorSetLayout) {
	mp_context=pContext;
	mp_resources=pResources;
	mp_vecDescriptorBinding=new std::vector<VkDescriptorSetLayoutBinding>();
	mp_descriptorUBO=nullptr;
	mp_descriptorSampler=nullptr;
	mp_descriptorSetLayout=descriptorSetLayout;
}

MFVkDescriptorSetLayout::~MFVkDescriptorSetLayout() {
	vkDestroyDescriptorSetLayout(*(mp_context->getVirtualDevice()), *mp_descriptorSetLayout, nullptr);
	delete mp_descriptorSetLayout;
	if(mp_descriptorSampler!=nullptr){
		delete mp_descriptorSampler;
	}
	if(mp_descriptorUBO!=nullptr){
		delete mp_descriptorUBO;
	}
}
uint32_t MFVkDescriptorSetLayout::addDescriptorSet(VkDescriptorSetLayoutBinding &descriptor){
	mp_vecDescriptorBinding->push_back(descriptor);
	return mp_vecDescriptorBinding->size();
}

uint32_t MFVkDescriptorSetLayout::addUBO(uint32_t binding){
	VkDescriptorSetLayoutBinding* uboLayoutBinding=new VkDescriptorSetLayoutBinding();
	uboLayoutBinding->binding = binding;
	uboLayoutBinding->descriptorCount = 1;
	uboLayoutBinding->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding->pImmutableSamplers = nullptr;
	uboLayoutBinding->stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	mp_vecDescriptorBinding->push_back(*uboLayoutBinding);
	if(mp_descriptorUBO!=nullptr){
		delete mp_descriptorUBO;
	}
	mp_descriptorUBO=uboLayoutBinding;
	return mp_vecDescriptorBinding->size();

}

uint32_t MFVkDescriptorSetLayout::addSampler(uint32_t binding){
	VkDescriptorSetLayoutBinding *samplerLayoutBinding=new VkDescriptorSetLayoutBinding();
	samplerLayoutBinding->binding = binding;
	samplerLayoutBinding->descriptorCount = 1;
	samplerLayoutBinding->descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding->pImmutableSamplers = nullptr;
	samplerLayoutBinding->stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	mp_vecDescriptorBinding->push_back(*samplerLayoutBinding);
	if(mp_descriptorSampler!=nullptr){
		delete mp_descriptorSampler;
	}
	mp_descriptorSampler=samplerLayoutBinding;
	return mp_vecDescriptorBinding->size();
}

bool MFVkDescriptorSetLayout::createDescriptorSetLayout(){
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(mp_vecDescriptorBinding->size());
	layoutInfo.pBindings = mp_vecDescriptorBinding->data();
	if (vkCreateDescriptorSetLayout(*(mp_context->getVirtualDevice()), &layoutInfo, nullptr, mp_descriptorSetLayout) != VK_SUCCESS) {
		mp_context->printErr("MFVkDescriptorSetLayout::createDescriptorSetLayout - failed to create descriptor set layout!");
		return false;
	}
	return true;
}

std::vector<VkDescriptorSetLayoutBinding>* MFVkDescriptorSetLayout::getDescriptors(){
	return mp_vecDescriptorBinding;
}
VkDescriptorSetLayoutBinding* MFVkDescriptorSetLayout::getUboDescriptor(){
	return mp_descriptorUBO;
}
VkDescriptorSetLayoutBinding* MFVkDescriptorSetLayout::getSamplerDescriptor(){
	return mp_descriptorSampler;
}
VkDescriptorSetLayoutBinding* MFVkDescriptorSetLayout::getDescriptor(uint32_t position){
	if(position < 0 || position > (mp_vecDescriptorBinding->size()-1)){
		mp_context->printErr("Can't return descriptor, position out of bounds!");
		return nullptr;
	}
	return &(mp_vecDescriptorBinding->at(position));
}

