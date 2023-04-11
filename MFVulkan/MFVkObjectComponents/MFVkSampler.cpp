/*
 * MFVkSampler.cpp
 *
 *  Created on: 19.08.2019
 *      Author: michl
 */

#include "../MFVkObjectComponents/MFVkSampler.h"

MFVkSampler::MFVkSampler(MFVkContext* pContext):
m_sampler(){
	mp_context=pContext;
	m_samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	m_samplerInfo.magFilter = VK_FILTER_LINEAR;
	m_samplerInfo.minFilter = VK_FILTER_LINEAR;
	m_samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	m_samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	m_samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	m_samplerInfo.anisotropyEnable = VK_FALSE;//TODO connected to pipeline?
	m_samplerInfo.maxAnisotropy = 16;
	m_samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	m_samplerInfo.unnormalizedCoordinates = VK_FALSE;
	m_samplerInfo.compareEnable = VK_FALSE;
	m_samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	m_samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	m_samplerInfo.minLod = 0;
	m_samplerInfo.maxLod = static_cast<float>(1);
	m_samplerInfo.mipLodBias = 0;
}

MFVkSampler::~MFVkSampler() {
	// TODO Auto-generated destructor stub
}

bool MFVkSampler::createSampler(){
	if(vkCreateSampler(*mp_context->getVirtualDevice(),&m_samplerInfo,nullptr,&m_sampler) != VK_SUCCESS){
		mp_context->printErr("Failed to create sampler!");
		return false;
	}
	return true;
}

void MFVkSampler::setMipLevels(uint32_t mipLevels){
    m_samplerInfo.maxLod = static_cast<float>(mipLevels);
}
