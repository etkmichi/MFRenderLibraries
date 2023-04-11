/*
 * MFVkDescriptorLayoutCreator.cpp
 *
 *  Created on: 18.09.2019
 *      Author: michl
 */

#include "MFVkDescriptorLayoutFactory.h"

MFVkDescriptorLayoutFactory::MFVkDescriptorLayoutFactory() {
	mp_defaultSetup=new DescriptorLayoutSetup();
	initDefaultSetup();
}


MFVkDescriptorLayoutFactory::~MFVkDescriptorLayoutFactory() {
	// TODO Auto-generated destructor stub
}

void MFVkDescriptorLayoutFactory::initDefaultSetup(){
	uint32_t  defaultSize = 4;
	mp_defaultSetup->descriptorBindings.resize(defaultSize);
	//4 bindings (model view projection matrices and single color)
	VkDescriptorSetLayoutBinding
		*binding1=&mp_defaultSetup->descriptorBindings.at(0),
		*binding2=&mp_defaultSetup->descriptorBindings.at(1),
		*binding3=&mp_defaultSetup->descriptorBindings.at(2),
		*binding4=&mp_defaultSetup->descriptorBindings.at(3);

	/*setup the model matrix binding*/
	binding1->binding=0;
	binding1->descriptorCount=1;
	binding1->stageFlags=VK_SHADER_STAGE_VERTEX_BIT;
	binding1->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	binding1->pImmutableSamplers=nullptr;

	/*setup the view matrix binding*/
	binding2->binding=1;
	binding2->descriptorCount=1;
	binding2->stageFlags=VK_SHADER_STAGE_VERTEX_BIT;
	binding2->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	binding2->pImmutableSamplers=nullptr;

	/*setup the projection matrix binding*/
	binding3->binding=2;
	binding3->descriptorCount=1;
	binding3->stageFlags=VK_SHADER_STAGE_VERTEX_BIT;
	binding3->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	binding3->pImmutableSamplers=nullptr;

	/*setup the color binding*/
	binding4->binding = 3;
	binding4->descriptorCount = 1;
	binding4->stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	binding4->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	binding4->pImmutableSamplers=nullptr;
}

DescriptorLayoutSetup* MFVkDescriptorLayoutFactory::getDefaultLayout(){
	return mp_defaultSetup;
}

bool MFVkDescriptorLayoutFactory::createDefaultLayout(VkDevice device, VkDescriptorSetLayout* pLayout){
	if(pLayout==nullptr || device == VK_NULL_HANDLE) return false;
	VkDescriptorSetLayoutCreateInfo info= {};
	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
	info.bindingCount=static_cast<uint32_t>(mp_defaultSetup->descriptorBindings.size()),
	info.pBindings = mp_defaultSetup->descriptorBindings.data();
	if(VK_SUCCESS!=vkCreateDescriptorSetLayout(device, &info, nullptr, pLayout)){
		return false;
	}
	return true;
}
