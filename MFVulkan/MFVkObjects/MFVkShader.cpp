/*
 * MFVkShader.cpp
 *
 *  Created on: 16.07.2019
 *      Author: michl
 */

#include "MFVkShader.h"

MFVkShader::MFVkShader(std::string path, std::string filename,MFVkContext* pContext):
  MFShader(path,filename,pContext)
{
	m_isShaderModuleCreated=false;
	mp_context=pContext;
	mp_shaderStageInfo = new VkPipelineShaderStageCreateInfo();
	mp_shaderModule = new VkShaderModule();
}

MFVkShader::MFVkShader(MFVkContext* pContext):
    MFShader(pContext)
{
	mp_context=pContext;
	mp_shaderStageInfo = new VkPipelineShaderStageCreateInfo();
	m_isShaderModuleCreated=false;
	mp_shaderModule = new VkShaderModule();
}

MFVkShader::~MFVkShader() {
	// TODO Auto-generated destructor stub
	delete mp_shaderStageInfo;
	if(m_isShaderModuleCreated==true){//TODO dynamic delete with function call -> set pointer to nullptr!
		destroyShaderModule();
	}
	delete mp_shaderModule;
}
void MFVkShader::destroyShaderModule(){
	vkDestroyShaderModule(*mp_context->getVirtualDevice(), *mp_shaderModule, nullptr);
}
VkShaderModule* MFVkShader::createShaderModule() {
	if(m_isShaderModuleCreated==true){
		mp_context->printInfo("MFVkShader::createShaderModule() - "
				"shader module was created before, it will be desroyed and recreated!");
		vkDestroyShaderModule(
				*mp_context->getVirtualDevice(),
				*mp_shaderModule,
				nullptr);
		m_isShaderModuleCreated=false;
		//TODO test if mp_shaderModule needs to be a new one
	}
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	if(!this->compile()){
		mp_context->printErr("MFVkShader - failed compiling shader!");
		return nullptr;
	}
	if(!this->loadShaderFromFile(m_binaryFilePath)){
		mp_context->printErr("MFVkShader - failed loading shader!");
		return nullptr;
	}
	createInfo.codeSize = m_shaderBinary.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(m_shaderBinary.data());


	if (vkCreateShaderModule(*mp_context->getVirtualDevice(), &createInfo, nullptr, mp_shaderModule) != VK_SUCCESS) {
		m_isShaderModuleCreated=false;
		mp_context->printErr("MFVkShader::createShaderModule() - failed to create shader module!");
		return nullptr;
	}
	mp_context->printInfo("MFVkShader::createShaderModule() - vulkan shader module created successfully! dataSize="+std::to_string(m_shaderBinary.size()));
	m_isShaderModuleCreated=true;
	m_isCompiled=true;
	return mp_shaderModule;
}

VkPipelineShaderStageCreateInfo* MFVkShader::createShaderStage(){
	VkShaderModule* shaderModule = createShaderModule();
	if(shaderModule==nullptr){
		mp_context->printErr("shader - failed to create shader module!");
		return nullptr;
	}
	mp_shaderStageInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	mp_shaderStageInfo->stage = m_stage;
	mp_shaderStageInfo->module = *mp_shaderModule;
	mp_shaderStageInfo->pName = "main";
	return mp_shaderStageInfo;
}
