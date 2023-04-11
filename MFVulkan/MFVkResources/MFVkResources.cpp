/*
 * MFVkResources.cpp
 *
 *  Created on: 19.07.2019
 *      Author: michl
 */

#include "MFVkResources.h"
#include "../MFPredefinedObjectSetups/MFVkDefaultSetup.h"

MFVkResources::MFVkResources(MFVkContext* pContext) :
MFRenderResources(pContext)
{
	mp_context=pContext;
	mp_defaultSetup=new MFVkDefaultSetup(mp_context,this);
	mp_swapChain=new MFVkSwapchainRes(mp_context,this);

	mp_commandManager = new MFVkCommandManager(mp_context,this);

	mp_colorImage=nullptr;//= new MFVkImage(mp_context,this);
	mp_depthImage=new MFVkImage(mp_context,this);

	mp_sampler = new MFVkSampler(mp_context);

	m_isInitialized=false;
	mp_vecImages=new std::vector<MFVkImage>();
	mp_sampler = new MFVkSampler(pContext);

	mp_renderPass=new MFVkRenderPass(mp_context,this);
	mp_framebuffer=new MFVkFramebuffer(mp_context,this);
	mp_descriptorManager = new MFVkDescriptorManager(mp_context, this);
	mp_pipelineManager = new MFVkPipelineManager(mp_context,this);
	mp_shaderManager = new MFVkShaderManager(mp_context,this);
	mp_objectManager = new MFVkObjectManager(mp_context,this);
	mp_vecObjectSetups=new std::vector<MFAbstractSetup*>();
	mp_vecObjectSetups->push_back(mp_defaultSetup);
}

MFVkResources::~MFVkResources() {
	this->printInfo("Deleting MFVkResources object!");
	delete mp_swapChain;
	delete mp_sampler;

	delete mp_commandManager;
	delete mp_colorImage;
	delete mp_depthImage;
	delete mp_objectManager;
	delete mp_vecObjectSetups;
}

bool MFVkResources::initResources(){
	//TODO use MFVkResourceManager as base class for all
	//managers and init with for loop
	if(m_isInitialized)return true;
	if(!initSwapChain_1())return false;

	if(!initRenderPass_2())return false;

	if(!initDescriptorSetups_3())return false;

	if(!initShader_3_1())return false;

	if(!initPipelineManager_4())return false;

	if(!initPipelineSetups_4_1())return false;

	if(!initCommandManager_5())return false;

	if(!initUBOManager_6_1())return false;

	if(!initObjectManager_6_2())return false;

	//	if(!initColorImage_6()){this->printInfo("resources - failed initColorImage_6!")
	//return false;}
	if(!initDepthImage_7(false))return false;

	if(!initFramebuffers_8()) return false;
//	if(!initSampler_9()){this->printInfo("resources - failed initSampler_9!");return false;}
	this->printInfo("MFVkResources::initResources() - init done!!!!");
	m_isInitialized=true;
	return true;
}

bool MFVkResources::recreateSwapChain(){
	int width = 0, height = 0;
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(mp_context->getWindow(), &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(*mp_context->getVirtualDevice());

	/**/
	this->printInfo("MFVkResources::recreateSwapChain() "
			"- recreation of swap chain - deletion of outdated resources");

	if(mp_colorImage!=nullptr)
		mp_colorImage->destroyVulkanResources();
	if(mp_depthImage!=nullptr)
		mp_depthImage->destroyVulkanResources();

	mp_framebuffer->destroyFramebuffers();

	mp_commandManager->freeBuffer();

	mp_pipelineManager->destroyPipelines();

	for(MFAbstractSetup* pSetup:*mp_vecObjectSetups){
		pSetup->destroyPipeline();
	}
	mp_renderPass->destroyVulkanResources();
	mp_swapChain->destroyImageViews();
	mp_swapChain->destroySwapChain();

	for(MFAbstractSetup* pSetup:*mp_vecObjectSetups){
		pSetup->destroyUBOs();//Take care to reinit and setup all scene objects which
		//are using the buffers
		pSetup->destroyDescriptorPool();
	}
//	mp_descriptorManager->destroyDescriptorPool();

	this->printInfo("MFVkResources::recreateSwapChain() - "
			"reinit of resources");

	if(!initSwapChain_1())return false;

	if(!initRenderPass_2())return false;
	for(MFAbstractSetup* pSetup:*mp_vecObjectSetups){
		if(!pSetup->reinitDescriptors())return false;
		//invalid pipeline layout
		//invalid render pass...
		if(!pSetup->reinitPipeline())return false;//TODO reinitPipeline failed!
	}

	if(!initPipelineManager_4())return false;//TODO remove pip mgr

	if(!mp_commandManager->createCommandBuffers())return false;

	//TODO if(!initUBOManager_6_1())return false;
	for(MFAbstractSetup* pSetup:*mp_vecObjectSetups){
		pSetup->reinitUBO();//TODO
	}

	if(!initObjectManager_6_2())return false;

	for(MFAbstractSetup* pSetup:*mp_vecObjectSetups){
		pSetup->reinitSceneObjects();
	}

	//	if(!initColorImage_6()){this->printInfo("resources - failed initColorImage_6!");return false;}
	if(!initDepthImage_7(true))return false;

	if(!initFramebuffers_8())return false;

//	if(!initFramebuffers_8()) return false;
//	if(!initSampler_9()){this->printInfo("resources - failed initSampler_9!");return false;}
	this->printInfo("MFVkResources::initResources() - init done!!!!");
	return true;
}

VkExtent2D MFVkResources::getSwapChainExtent(){return mp_swapChain->getExtent();}

MFVkImage* MFVkResources::getColorImage(){
	return mp_colorImage;
}
MFVkImage* MFVkResources::getDepthImage(){
	return mp_depthImage;
}

VkSampler* MFVkResources::getTextureSampler(){return &(mp_sampler->getSampler());}

bool MFVkResources::initUBOManager_6_1(){
	for(MFAbstractSetup* pObjectSetup: *mp_vecObjectSetups){
		if(!pObjectSetup->initUBOManager()){
			this->printErr("MFVkResources::initUBOManager_6_1() - failed 1!");
			return false;
		}
	}
	return true;
}

bool MFVkResources::initObjectManager_6_2(){
	mp_objectManager->setMaxObjectCount(
			mp_context->getSetup()->m_sceneSetupMaxObjectCount);
	if(!mp_objectManager->initObjectManager()){
		this->printErr("resources - object manager initialication failed!");
		return false;
	}

	return true;
}

bool MFVkResources::initSwapChain_1(){
	this->printInfo(" MFVkResources::initSwapChain_1()");
	if(!mp_swapChain->initSwapChain()){
		this->printErr("MFVkResources::initSwapChain_1() - failed!");
		return false;
	}
	m_isInitialized = true;
	return true;
}


bool MFVkResources::initRenderPass_2(){
	mp_renderPass->cleanSetup();
	mp_renderPass->addDepthStencilAttachment();
	mp_renderPass->addColorAttachment();
	if(!mp_renderPass->createRenderPass(true)){
		this->printErr("MFAbstractSetup::createRenderPass() - "
				"render pass creation failed!");
		return false;
	}
	for(MFAbstractSetup* pSetup:*mp_vecObjectSetups){
		pSetup->setRenderPass(mp_renderPass->getRenderPass());
	}
	return true;
}

bool MFVkResources::initDescriptorSetups_3(){
	this->printInfo("MFVkResources::initDescriptorSetManager_3()");
//	mp_descriptorManager->setDescriptorSetCount(
//			mp_context->getSetup()->m_sceneSetupMaxObjectCount);
//	if(!mp_descriptorManager->initDescriptorManager()){
//		this->printErr("MFVkResources::initDescriptorSetManager_3()"
//					" - failed initDescriptorSetManager_3!");
//		return false;
//	}
	bool ret=true;
	for(MFAbstractSetup* pSetup:*mp_vecObjectSetups){
		bool ret2=pSetup->initDescriptorSetup();
		if(!ret2){
			this->printErr("MFVkResources::initDescriptorSetups_3() - "
					"failed to initDescriptorSetup of MFAbstractSetup!");
			ret=false;
		}
	}
	return ret;
}

bool MFVkResources::initShader_3_1(){
	this->printInfo("MFVkResources::initShaderManager_3_1()");
//	if(!mp_shaderManager->initShaderManager()){
//		this->printErr("MFVkResources::initShaderManager_3_1() - failed!");
//		return false;
//	}
	bool ret=true;
	for(MFAbstractSetup* pSetup:*mp_vecObjectSetups){
		bool ret2=pSetup->initShader();
		if(!ret2){
			this->printErr("MFVkResources::initDescriptorSetups_3() - "
					"failed to initDescriptorSetup of MFAbstractSetup!");
			ret=false;
		}
	}
	return ret;
}

bool MFVkResources::initPipelineManager_4(){
	this->printInfo("MFVkResources::initPipelineManager_4()");
	if(!mp_pipelineManager->initPipelineManager()){
		this->printInfo("MFVkResources::initPipelineManager_4() - "
				"failed!");
		return false;
	}
	return true;
}

bool MFVkResources::initPipelineSetups_4_1(){
	bool ret=true;
	for(MFAbstractSetup* pSetup:*mp_vecObjectSetups){
		bool ret2=pSetup->initPipeline();
		if(!ret2){
			this->printErr("MFVkResources::initPipelineManager_4() - "
					"failed to init pipeline of MFAbstractSetup!");
			ret=false;
		}
	}
	return ret;
}

bool MFVkResources::initCommandManager_5(){
	this->printInfo("MFVkResources::initCommandManager_5()");
	mp_commandManager->setBufferCount(mp_context->getSetup()->m_swapChainImageCount);
	return mp_commandManager->init();
}

bool MFVkResources::initColorImage_6(){
	this->printInfo("MFVkResources - init color image - no color image used!");
//	if(!MFVkImage::setupColorImage(mp_context,this,mp_colorImage))return false;
//	if(!mp_colorImage->createImage())return false;
//	if(!mp_colorImage->createImageView())return false;
//	mp_colorImage->transferImageLayout(VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	return true;
}

bool MFVkResources::initDepthImage_7(bool recreate){
	this->printInfo("MFVkResources::initDepthImage_7()");
	if(!MFVkImage::setupDepthImage(mp_context,this,mp_depthImage)){
		this->printErr("MFVkResources::initDepthImage_7() - "
				"setupDepthImage failed!");
		return false;
	}
	if(!mp_depthImage->createImage()){
		this->printErr("MFVkResources::initDepthImage_7() - "
				"createImage failed!");
		return false;
	}
	if(!mp_depthImage->createImageView()){
		this->printErr("MFVkResources::initDepthImage_7() - "
				"createImageView failed!");
		return false;
	}
	mp_depthImage->transferImageLayout(VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	if(!recreate){
		mp_framebuffer->addAttachment(mp_depthImage);
	}
	return true;
}

bool MFVkResources::initFramebuffers_8(){
	this->printInfo("MFVkResources::initFramebuffers_8()");
	if(!mp_framebuffer->initFramebuffer()){
		this->printErr("MFVkResources::initFramebuffers_8() - failed!");
		return false;
	}
	return true;
}

bool MFVkResources::initSampler_9(){
	this->printInfo("resources - creating img sampler - no sampler used");
//	if(!mp_sampler->createSampler()){
//		return false;
//	}
	return true;
}

void MFVkResources::addObjectSetup(MFAbstractSetup* pSetup){
	mp_vecObjectSetups->push_back(pSetup);
}

VkFormat MFVkResources::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features){
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(*(mp_context->getPhysicalDevice()), format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		} else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");

}
