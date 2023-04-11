/*
 * MFVkDescriptorManager.cpp
 *
 *  Created on: 03.09.2019
 *      Author: michl
 */

#include "MFVkDescriptorManager.h"

MFVkDescriptorManager::MFVkDescriptorManager(
		MFVkContext *pContext, MFVkResources *pResources) {
	mp_resources=pResources;
	mp_context=pContext;
	mp_vecDescriptorPool=new std::vector<VkDescriptorPool>(1);
	mp_vecDescriptorSet = nullptr;
	mp_vecLayoutSetup = new std::vector<MFVkDescriptorSetLayout*>();
	mp_vecDescriptorSetLayout = new std::vector<VkDescriptorSetLayout>(1);/*contains all different layouts*/

	//TODO layout setup
	mp_vecLayoutSetup->push_back(new MFVkDescriptorSetLayout(pContext,pResources,&(mp_vecDescriptorSetLayout->at(0))));
	mp_descriptorPool=new VkDescriptorPool();
	mp_objectsPool=new VkDescriptorPool();
	m_nextDescriptor=0;
	mp_defaultSetup=mp_resources->getDefaultObjectSetup();
}

MFVkDescriptorManager::~MFVkDescriptorManager() {
	destroyDescriptorPool();
	delete mp_descriptorPool;
	for(MFVkDescriptorSetLayout* lo : (*mp_vecLayoutSetup)){
		delete lo;
	}
	delete mp_vecLayoutSetup;
}

void MFVkDescriptorManager::destroyDescriptorPool(){
    vkDestroyDescriptorPool(*mp_context->getVirtualDevice(), *mp_descriptorPool, nullptr);
}

bool MFVkDescriptorManager::initDescriptorManager(){
//	mp_context->printInfo("MFVkDescriptorManager::initDescriptorManager() - "
//			"Creating default descriptor layout!");
	/*TODO init MFVkDefaultSetup*/
//	if(!mp_defaultSetup->initDescriptorSetup()){
//		mp_context->printErr("MFVkDescriptorManager::initDescriptorManager() - "
//				"failed to init default object setup!");
//		return false;
//	}
	return true;
}

bool MFVkDescriptorManager::createDescriptorSetLayout(){
	for(MFVkDescriptorSetLayout* layout:(*mp_vecLayoutSetup)){
		if(!layout->createDescriptorSetLayout())
			return false;
	}
	return true;
}

MFVkDescriptorSetLayout* MFVkDescriptorManager::addLayoutSetup(){
	mp_vecDescriptorSetLayout->resize(mp_vecDescriptorSetLayout->size()+1);
	MFVkDescriptorSetLayout
		*pSetup=new MFVkDescriptorSetLayout(mp_context,mp_resources,&(mp_vecDescriptorSetLayout->at(mp_vecDescriptorSetLayout->size()-1)));
	mp_vecLayoutSetup->push_back(pSetup);
	return pSetup;
}

bool MFVkDescriptorManager::createDescriptorPool(){
	std::array<VkDescriptorPoolSize,2> poolSizes={};

	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(m_descriptorCount);
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(m_samplerCount);/*count of swap chain images*/

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(m_descriptorCount);

	if (vkCreateDescriptorPool(*mp_context->getVirtualDevice(), &poolInfo, nullptr, mp_descriptorPool) != VK_SUCCESS) {
		mp_context->printErr("failed to create descriptor pool!");
		return false;
	}
	return true;
}

void MFVkDescriptorManager::setSamplerCount(uint32_t samplerCount){
	m_samplerCount=samplerCount;
}

bool MFVkDescriptorManager::getNextDescriptorIndex(uint32_t &index){
	index=m_nextDescriptor;
	if(index > mp_vecDescriptorSet->size()){
		mp_context->printErr("Not enough descriptors available!");
		return false;
	}
	m_nextDescriptor++;

	return true;
}

VkDescriptorSet* MFVkDescriptorManager::getDescriptor(uint32_t index){
	if(index > mp_vecDescriptorSet->size()){
		mp_context->printErr("Not enough descriptors available!");
		return nullptr;
	}

	return &(mp_vecDescriptorSet->at(index));
}



