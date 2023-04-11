/*
 * MFVkSampler.h
 *
 *  Created on: 19.08.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKRESOURCES_MFVKSAMPLER_H_
#define MFVULKAN_MFVKRESOURCES_MFVKSAMPLER_H_
class MFVkSampler;

#include "vulkan/vulkan.h"
#include "../MFVkContext/MFVkContext.h"
#include "MFObjects/MFObject.h"
class MFVkSampler {
private:
    VkSamplerCreateInfo
		m_samplerInfo = {};
    VkSampler
		m_sampler;
    MFVkContext
		*mp_context;
public:
	MFVkSampler(MFVkContext* pContext);
	virtual ~MFVkSampler();
	VkSamplerCreateInfo* getSamplerCreateInfo(){return &m_samplerInfo;};
	bool createSampler();
	VkSampler& getSampler(){return m_sampler;};
	VkSamplerCreateInfo& getCreateInfo(){return m_samplerInfo;};
	void setMipLevels(uint32_t mipLevels);
};

#endif /* MFVULKAN_MFVKRESOURCES_MFVKSAMPLER_H_ */
