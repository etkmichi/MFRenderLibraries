/*
 * MFVkShader.h
 *
 *  Created on: 16.07.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKSHADER_H_
#define MFVULKAN_MFVKSHADER_H_

class MFVkShader;

#include "../MFVkContext/MFVkContext.h"
#include "../../MFRenderClasses/MFShaders/MFShader.h"
#include <vulkan/vulkan.h>

class MFVkShader: public MFShader {
private:
	MFVkContext*
		mp_context;

	VkShaderModule*
		mp_shaderModule;

	VkPipelineShaderStageCreateInfo*
		mp_shaderStageInfo;
	bool
		m_isShaderModuleCreated;

	VkShaderStageFlagBits
		m_stage=VK_SHADER_STAGE_VERTEX_BIT;

public:
	MFVkShader(std::string path, std::string filename,MFVkContext* pContext);
	MFVkShader(MFVkContext* pContext);
	virtual ~MFVkShader();
	VkShaderModule* createShaderModule();
	void destroyShaderModule();

	/**
	 * Creates the shader module and fills the VkPipelineShaderStageCreateInfo.
	 * @return the filled VkPipelineShaderStageCreateInfo pointer.
	 */
	VkPipelineShaderStageCreateInfo* createShaderStage();
	void setShaderStage(VkShaderStageFlagBits stage){
		m_stage=stage;
	}
	VkDescriptorSetLayout* getDescriptorSetLayout(uint32_t count);
};

#endif /* MFVULKAN_MFVKSHADER_H_ */
