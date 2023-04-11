/*
 * MFVkPipeline.cpp
 *
 *  Created on: 15.07.2019
 *      Author: michl
 */

#include "../MFVkObjectComponents/MFVkPipeline.h"

MFVkPipeline::MFVkPipeline(MFVkContext* pContext,PipelineSetup* pipelineSetup){
	init();
	mp_context=pContext;
	mp_pipelineSetup = pipelineSetup;
	mp_layout=pipelineSetup->pPipelineLayout;
}
MFVkPipeline::MFVkPipeline(MFVkContext* pContext){
	init();
	mp_context=pContext;
	mp_pipelineSetup = new PipelineSetup();

	mp_layout = new VkPipelineLayout();
	mp_pipelineSetup->pPipelineLayout=mp_layout;
	mp_pipelineSetup->useCustomLayout=false;
}
MFVkPipeline::~MFVkPipeline() {
	m_vecShaderStages.clear();
	destroyVulkanResources();
	if(mp_graphicsPipeline!=nullptr)
		delete mp_graphicsPipeline;
	if(mp_layout!=nullptr)
		delete mp_layout;
	//TODO delete other resources

	delete mp_vertexInputState;
	delete mp_inputAssemblyState;
	delete mp_viewportState;
	delete mp_rasterizationState;
	delete mp_multisampleState;
	delete mp_depthStencilState;
	delete mp_colorBlendAttachmentState;
	delete mp_colorBlendState;
	delete mp_pipelineCreateInfo ;
	delete mp_layoutInfo ;
}

void MFVkPipeline::init(){
	m_customShaderStage=false;
	m_customVertexInputState=false;
	m_customInputAssembly=false;
	m_customViewportState=false;
	m_customRasterizationState=false;
	m_customMultisampleState=false;
	m_customDepthStencilState=false;
	m_customColorBlendAttachment=false;
	m_customColorBlendState=false;
	m_customLayoutInfo=false;
	m_customPipelineInfo=false;
	m_resourcesInitiliced=false;

	mp_vertexInputState = new VkPipelineVertexInputStateCreateInfo();
	mp_inputAssemblyState = new VkPipelineInputAssemblyStateCreateInfo();
	mp_viewportState = new VkPipelineViewportStateCreateInfo();
	mp_rasterizationState = new VkPipelineRasterizationStateCreateInfo();
	mp_multisampleState = new VkPipelineMultisampleStateCreateInfo();
	mp_depthStencilState = new VkPipelineDepthStencilStateCreateInfo();
	mp_colorBlendAttachmentState = new VkPipelineColorBlendAttachmentState();
	mp_colorBlendState = new VkPipelineColorBlendStateCreateInfo();
	mp_pipelineCreateInfo = new VkGraphicsPipelineCreateInfo();
	mp_layoutInfo = new VkPipelineLayoutCreateInfo();
	mp_graphicsPipeline = new VkPipeline();

}
void MFVkPipeline::destroyVulkanResources(){
	vkDestroyPipeline(*mp_context->getVirtualDevice(), *mp_graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(*mp_context->getVirtualDevice(), *mp_layout, nullptr);
}

PipelineSetup* MFVkPipeline::getPipelineSetup(){
	return mp_pipelineSetup;
}

bool MFVkPipeline::initPipelineResources(){
	if(mp_layout==nullptr){
		mp_layout=mp_pipelineSetup->pPipelineLayout;
	}
	mp_context->printInfo("MFVkPipeline::initPipelineResources()");
	try{
		/*Fills information structure with shader relevant data - meta information and pointer to compiled shader binaries.*/
		if(!m_customShaderStage)
			createShaderStages();/*creates m_vecShaderStages*/
		if(!m_customVertexInputState)
			createVertexInputState();/*fills mp_vertexInputState*/
		if(!m_customInputAssembly)
			createInputAssemblyState();
		if(!m_customViewportState)
			createViewportState();
		if(!m_customRasterizationState)
			createRasterizationState();
		if(!m_customMultisampleState)
			createMultisampleState();
		if(!m_customDepthStencilState)
			createDepthStencilState();
		if(!m_customColorBlendAttachment)
			createColorBlendAttachmentState();/*Only for createColorBlendState()*/
		if(!m_customColorBlendState)
			createColorBlendState();
		if(!m_customLayoutInfo)/*Used for shader manipulation without recreating shaders. Used for passing information to shaders (Trafos samplers)*/
			createLayoutInfo();
		//TODO dynamically changable variables VkPipelineDynamicStateCreateInfo -> functions for changing -> data must be given at drawing time!
	}catch(std::exception& e){
		mp_context->printErr("MFVkPipeline::initPipelineResources() - sth went wrong during pipline resource initialization!");
		m_resourcesInitiliced=false;
		return false;
	}
	m_resourcesInitiliced=true;
	return true;

}
bool MFVkPipeline::createPipelineLayout(){
	if(vkCreatePipelineLayout(
			*mp_context->getVirtualDevice(),
			mp_layoutInfo,
			nullptr,
			mp_layout) != VK_SUCCESS) {
		mp_context->printErr("MFVkPipeline::initPipeline() vkCreatePipelineLayout call failed!");
		return false;
	}
	mp_pipelineCreateInfo->layout = *mp_layout;
	return true;
}

bool MFVkPipeline::createPipeline(){
	createPipelineInfo();
	if(vkCreateGraphicsPipelines(
		*(mp_context->getVirtualDevice()),
		VK_NULL_HANDLE,
		1,
		mp_pipelineCreateInfo,
		nullptr,
		mp_graphicsPipeline) != VK_SUCCESS) {
		mp_context->printErr("MFVkPipeline::initPipeline() - "
				"vkCreateGraphicsPipelines call failed!");
		return false;
	}
	return true;
}

bool MFVkPipeline::initPipeline(){
	mp_context->printInfo("MFVkPipeline::initPipeline() creating pipeline");
	if(m_resourcesInitiliced==false){
		mp_context->printErr("MFVkPipeline::initPipeline() resources must be initialiced (MFVkPipeline::initPipelineResources()) successfully before calling this method!");
		return false;
	}
	if(!mp_pipelineSetup->useCustomLayout){//if not custom
		if(mp_layout==nullptr){
			mp_layout=new VkPipelineLayout();
		}
		if (!createPipelineLayout())return false;
	}

	if(!m_customPipelineInfo){
		mp_context->printInfo("MFVkPipeline::initPipeline() - filling crate info");
		createPipelineInfo();
	}
	//TODO add pipelineCreateInfo addPipeline create multiple pipelines
	mp_context->printInfo("MFVkPipeline::initPipeline() - creating vulkan graphics pipeline");
	try{
		if (!createPipeline()){return false;}
	}catch(std::exception& e){
		mp_context->printErr("MFVkPipeline::initPipeline() vkCreateGraphicsPipelines call failed! (2) "+std::string(e.what()));
		return false;
	}

	mp_context->printInfo("MFVkPipeline::initPipeline() - creating vulkan graphics pipeline done!");
//TODO when to delete the shader modules? -> Vk doc says can be deleted after creating but its not a must...
//	MFVkShader* shader=(MFVkShader*)(mp_pipelineSetup->shaderVertexPointer);
//	shader->destroyShaderModule();
//	MFVkShader* shader2=(MFVkShader*)(mp_pipelineSetup->shaderFragmentPointer);
//	shader2->destroyShaderModule();
    return true;
}

std::vector<VkPipelineShaderStageCreateInfo> MFVkPipeline::createShaderStages(){
	mp_context->printInfo("MFVkPipeline::createShaderStages()");
	m_shaderStageCount = mp_pipelineSetup->shaderVertCount+mp_pipelineSetup->shaderFragCount;
	if(m_shaderStageCount==0){
		mp_context->printInfo("MFVkPipeline::createShaderStages() - no shader added to the pipeline!!");
	}
	m_vecShaderStages.clear();

	/*Iterate through shader and fill the array with the shader stage info*/
	for(uint32_t i=0 ; i<mp_pipelineSetup->shaderVertCount ; i++){
		mp_context->printInfo("MFVkPipeline::createShaderStages() - setting up vertex shader");
		MFVkShader* shader=(MFVkShader*)(mp_pipelineSetup->shaderVertexPointer);
		VkPipelineShaderStageCreateInfo *pShader=shader->createShaderStage();
		pShader->stage=VK_SHADER_STAGE_VERTEX_BIT;
		if(pShader==nullptr){
			mp_context->printErr("pipeline - failed to create a vertex shader!");
			throw("pipeline - failed to create a vertex shader!");//TODO return false
		}
		m_vecShaderStages.push_back(*pShader);
	}

	for(uint32_t i=0 ; i<mp_pipelineSetup->shaderFragCount ; i++){
		mp_context->printInfo("MFVkPipeline::createShaderStages() - setting up fragment shader");
		MFVkShader* shader=(MFVkShader*)(mp_pipelineSetup->shaderFragmentPointer);
		VkPipelineShaderStageCreateInfo *pShader = shader->createShaderStage();
		pShader->stage=VK_SHADER_STAGE_FRAGMENT_BIT;
		if(pShader==nullptr){
			mp_context->printErr("pipeline - failed to create a fragment shader!");
			throw("pipeline - failed to create a fragment shader!");
		}
		m_vecShaderStages.push_back(*pShader);
	}
	return m_vecShaderStages;
}

VkPipelineVertexInputStateCreateInfo* MFVkPipeline::createVertexInputState(){
	mp_vertexInputState->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
//TODO add bidning description function for other kind of vertices maybe interface

	mp_vertexInputState->vertexBindingDescriptionCount = mp_pipelineSetup->bindingDescCount;
	mp_vertexInputState->vertexAttributeDescriptionCount = mp_pipelineSetup->bindingsAttributeDescCount;
	mp_vertexInputState->pVertexBindingDescriptions = mp_pipelineSetup->bindingDescriptionsPointer;
	mp_vertexInputState->pVertexAttributeDescriptions = mp_pipelineSetup->bindingAttributeDescPointer;
	return mp_vertexInputState;
}

VkPipelineInputAssemblyStateCreateInfo* MFVkPipeline::createInputAssemblyState(){
	mp_inputAssemblyState->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	mp_inputAssemblyState->topology = mp_pipelineSetup->AssemblyTopology;
	mp_inputAssemblyState->primitiveRestartEnable = mp_pipelineSetup->AssemblyPrimitiveRestartEnable;

	return mp_inputAssemblyState;
}

VkPipelineViewportStateCreateInfo* MFVkPipeline::createViewportState(){
	mp_viewportState->sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	mp_viewportState->viewportCount = mp_pipelineSetup->viewportCount;
	mp_viewportState->pViewports = mp_pipelineSetup->viewportsPointer;
	mp_viewportState->scissorCount = mp_pipelineSetup->viewportScissorCount;
	mp_viewportState->pScissors = mp_pipelineSetup->viewportScissors;
	return mp_viewportState;
}

VkPipelineRasterizationStateCreateInfo* MFVkPipeline::createRasterizationState(){
	mp_rasterizationState->sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	mp_rasterizationState->depthClampEnable = mp_pipelineSetup->RasterizationDepthClampEnable;
	mp_rasterizationState->rasterizerDiscardEnable = mp_pipelineSetup->RasterizationRasterizerDiscarEnable;
	mp_rasterizationState->polygonMode = mp_pipelineSetup->RasterizationPolygonMode;
	mp_rasterizationState->lineWidth = mp_pipelineSetup->RasterizationLineWidth;
	mp_rasterizationState->cullMode = mp_pipelineSetup->RasterizationCullMode;
	mp_rasterizationState->frontFace = mp_pipelineSetup->RasterizationFrontFace;
	mp_rasterizationState->depthBiasEnable = mp_pipelineSetup->RasterizationDepthBiasEnable;
	mp_rasterizationState->depthBiasConstantFactor = 0;
	mp_rasterizationState->depthBiasClamp = 0;
	mp_rasterizationState->depthBiasSlopeFactor = 0;
	mp_rasterizationState->flags = 0;
	return mp_rasterizationState;
}

VkPipelineMultisampleStateCreateInfo* MFVkPipeline::createMultisampleState(){
	mp_multisampleState->sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	mp_multisampleState->pNext = NULL;
	mp_multisampleState->flags = 0;
	mp_multisampleState->pSampleMask = NULL;
	mp_multisampleState->sampleShadingEnable = mp_pipelineSetup->MultisampleSampleShadingEnabled;
	mp_multisampleState->rasterizationSamples = mp_pipelineSetup->MultisampleRasterizationSamples;
	mp_multisampleState->alphaToCoverageEnable = VK_FALSE;
	mp_multisampleState->alphaToOneEnable = VK_FALSE;
	mp_multisampleState->minSampleShading = 0.0;
	return mp_multisampleState;
}

VkPipelineDepthStencilStateCreateInfo* MFVkPipeline::createDepthStencilState(){
	mp_depthStencilState->sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	mp_depthStencilState->depthTestEnable = mp_pipelineSetup->depthTestEnable;
	mp_depthStencilState->depthWriteEnable = mp_pipelineSetup->depthWriteEnable;
	mp_depthStencilState->depthCompareOp = mp_pipelineSetup->depthCompareOp;
	mp_depthStencilState->depthBoundsTestEnable = mp_pipelineSetup->depthBoundsTestEnable ;
	mp_depthStencilState->stencilTestEnable = mp_pipelineSetup->depthStencilTestEnable;


	return mp_depthStencilState;
}

VkPipelineColorBlendAttachmentState* MFVkPipeline::createColorBlendAttachmentState(){
	mp_colorBlendAttachmentState->colorWriteMask =mp_pipelineSetup->ColorBlendAttachmentColorWriteMask;
	mp_colorBlendAttachmentState->blendEnable = mp_pipelineSetup->ColorBlendAttachmentBlendEnable;
	mp_colorBlendAttachmentState->colorWriteMask = 0xf;//TODO
	mp_colorBlendAttachmentState->alphaBlendOp = VK_BLEND_OP_ADD;
	mp_colorBlendAttachmentState->colorBlendOp = VK_BLEND_OP_ADD;
	mp_colorBlendAttachmentState->srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	mp_colorBlendAttachmentState->dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	mp_colorBlendAttachmentState->srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	mp_colorBlendAttachmentState->dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	return mp_colorBlendAttachmentState;
}

VkPipelineColorBlendStateCreateInfo* MFVkPipeline::createColorBlendState(){
	mp_colorBlendState->sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	mp_colorBlendState->logicOpEnable = mp_pipelineSetup->ColorBlendinglogicOpEnable;
	mp_colorBlendState->logicOp = mp_pipelineSetup->ColorBlendinglogicOp;
	mp_colorBlendState->attachmentCount = mp_pipelineSetup->ColorBlendingAttachmentCount;
	mp_colorBlendState->pAttachments = mp_colorBlendAttachmentState;
	mp_colorBlendState->blendConstants[0] = mp_pipelineSetup->ColorBlendingBlendConstant1;
	mp_colorBlendState->blendConstants[1] = mp_pipelineSetup->ColorBlendingBlendConstant2;
	mp_colorBlendState->blendConstants[2] = mp_pipelineSetup->ColorBlendingBlendConstant3;
	mp_colorBlendState->blendConstants[3] = mp_pipelineSetup->ColorBlendingBlendConstant4;
	return mp_colorBlendState;
}

VkPipelineLayoutCreateInfo* MFVkPipeline::createLayoutInfo(){
	mp_layoutInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	mp_layoutInfo->pNext=nullptr;
	mp_layoutInfo->pushConstantRangeCount=0;
	mp_layoutInfo->pPushConstantRanges=nullptr;
	if(mp_pipelineSetup->useCustomDescriptorLayouts){
		mp_layoutInfo->setLayoutCount =mp_pipelineSetup->DescriptorSetLayoutCount;
		mp_layoutInfo->pSetLayouts = mp_pipelineSetup->DescriptorSetLayoutPointer;
	}else{
		std::vector<VkDescriptorSetLayout> *pLayouts =//TODO use default layout
					mp_context->getResources()->getObjectManager()->
					getDescriptorManager()->getVecLayout();
		mp_layoutInfo->setLayoutCount = pLayouts->size();
		mp_layoutInfo->pSetLayouts = pLayouts->data();
	}
	return mp_layoutInfo;
}

VkGraphicsPipelineCreateInfo* MFVkPipeline::createPipelineInfo(){
	/*Combine all created resource in the VkGraphics...*/
	mp_pipelineCreateInfo->sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	mp_pipelineCreateInfo->pNext = NULL;
	mp_pipelineCreateInfo->basePipelineIndex = 0;
	mp_pipelineCreateInfo->flags = 0;
	mp_pipelineCreateInfo->pTessellationState = nullptr;
	mp_pipelineCreateInfo->pDynamicState = nullptr;

	mp_pipelineCreateInfo->stageCount = m_vecShaderStages.size();
	mp_pipelineCreateInfo->pStages = m_vecShaderStages.data();
	mp_pipelineCreateInfo->pVertexInputState = mp_vertexInputState;
	mp_pipelineCreateInfo->pInputAssemblyState = mp_inputAssemblyState;
	//TODO implement tesselation state and pDynamicState
	mp_pipelineCreateInfo->pViewportState = mp_viewportState;
	mp_pipelineCreateInfo->pRasterizationState = mp_rasterizationState;
	mp_pipelineCreateInfo->pMultisampleState = mp_multisampleState;
	mp_pipelineCreateInfo->pDepthStencilState = mp_depthStencilState;
	mp_pipelineCreateInfo->pColorBlendState = mp_colorBlendState;
	mp_pipelineCreateInfo->layout = *mp_layout;
	mp_pipelineCreateInfo->renderPass = *mp_pipelineSetup->pRenderPass;
	mp_pipelineCreateInfo->subpass = 0;/*TODO test subpass*/
	mp_pipelineCreateInfo->basePipelineHandle = VK_NULL_HANDLE;
	return mp_pipelineCreateInfo;
}




