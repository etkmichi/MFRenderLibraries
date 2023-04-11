/*
 * MFVkRenderPass.cpp
 *
 *  Created on: 07.08.2019
 *      Author: michl
 */

#include "MFVkRenderPass.h"

MFVkRenderPass::MFVkRenderPass(MFVkContext *pContext, MFVkResources *pResources){
	mp_context=pContext;
	mp_resources=pResources;
	mp_vecAttachments = new std::vector<MFRenderPassAttachment*>();
	mp_vecAttachmentDescriptions = new std::vector<VkAttachmentDescription>();
	mp_vecInputAttachmentRefs = new std::vector<VkAttachmentReference>();
	mp_vecColorAttachmentRefs = new std::vector<VkAttachmentReference>();
	mp_vecPreserveAttachmentRefs = new std::vector<VkAttachmentReference>();
	mp_vecResolveAttachmentRefs = new std::vector<VkAttachmentReference>();
	mp_vecSubpasses=new std::vector<MFVkSubpass*>();
	mp_depthStencilAttachment=nullptr;
	mp_renderPass=new VkRenderPass();
	mp_vecSubpassDescriptions = new std::vector<VkSubpassDescription>();
	mp_buffer=nullptr;
	mp_bufferMemory=nullptr;
}

MFVkRenderPass::~MFVkRenderPass(){
	delete mp_vecAttachmentDescriptions;
	for(MFRenderPassAttachment* attachment:(*mp_vecAttachments)){
		if(attachment->deleteDescriptionInDestructor){
			delete attachment->pDescription;
		}
		delete attachment;
	}
	delete mp_vecAttachments;
	delete mp_vecResolveAttachmentRefs;
	delete mp_vecPreserveAttachmentRefs;
	delete mp_vecColorAttachmentRefs;
	delete mp_vecInputAttachmentRefs;
	for(MFVkSubpass* subpass:(*mp_vecSubpasses)){
		delete subpass;
	}
	delete mp_vecSubpasses;
	destroyVulkanResources();
	delete mp_renderPass;
}

void MFVkRenderPass::destroyVulkanResources(){
	vkDestroyRenderPass(*(mp_context->getVirtualDevice()),*mp_renderPass, nullptr);
}

void MFVkRenderPass::cleanSetup(){
	for(MFVkSubpass* subpass:(*mp_vecSubpasses)){
		delete subpass;
	}
	mp_vecSubpasses->clear();
	mp_vecInputAttachmentRefs->clear();
	mp_vecColorAttachmentRefs->clear();
	mp_vecResolveAttachmentRefs->clear();
	mp_vecPreserveAttachmentRefs->clear();
	for(MFRenderPassAttachment* attachment:(*mp_vecAttachments)){
		if(attachment->deleteDescriptionInDestructor){
			delete attachment->pDescription;
		}
		delete attachment;
	}
	mp_vecAttachments->clear();
	mp_depthStencilAttachment=nullptr;
	mp_vecAttachmentDescriptions->clear();

}
MFRenderPassAttachment* MFVkRenderPass::addAttachment(
		VkAttachmentDescription &description,
		const MFAttachmentSubpassBinding &binding,
		VkImageLayout &layout){
	mp_context->printInfo("render pass - adding attachment");
	MFRenderPassAttachment* attachment=new MFRenderPassAttachment();
	attachment->pDescription=&description;


	mp_vecAttachmentDescriptions->push_back(description);
	uint32_t index=mp_vecAttachmentDescriptions->size()-1;

	attachment->descriptionReference.attachment = index;
	attachment->descriptionReference.layout=layout;
	switch(binding){
	case MF_VK_ATTACHMENT_COLOR:
		mp_vecColorAttachmentRefs->push_back(attachment->descriptionReference);
		break;
	case MF_VK_ATTACHMENT_DEPTH_STENCIL:
		mp_depthStencilAttachment = attachment;
		break;
	case MF_VK_ATTACHMENT_INPUT:
		mp_vecInputAttachmentRefs->push_back(attachment->descriptionReference);
		break;
	case MF_VK_ATTACHMENT_PRESERVE:
		mp_vecPreserveAttachmentRefs->push_back(attachment->descriptionReference);
		break;
	case MF_VK_ATTACHMENT_RESOLVE:
		mp_vecResolveAttachmentRefs->push_back(attachment->descriptionReference);
		break;
	}
	mp_vecAttachments->push_back(attachment);
	return attachment;
}

MFRenderPassAttachment* MFVkRenderPass::addColorAttachment(){
	mp_context->printInfo("MFVkRenderPass::addColorAttachment()");
	MFRenderPassAttachment* attachment=new MFRenderPassAttachment();

//    if(mp_context->getSetup()->m_msaaSamples != VK_SAMPLE_COUNT_1_BIT){
	//TODO connect to color image
//        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//    }else{
//    }
	VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = mp_resources->getSwapchain()->getImageFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;//mp_context->getSetup()->m_msaaSamples;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	colorAttachment.flags=0;
	mp_vecAttachmentDescriptions->push_back(colorAttachment);
    attachment->pDescription=&(mp_vecAttachmentDescriptions->data()
    		[mp_vecAttachmentDescriptions->size()-1]);
	attachment->subpassBinding=MF_VK_ATTACHMENT_COLOR;
	attachment->descriptionReference.attachment =
			mp_vecAttachmentDescriptions->size()-1;
	attachment->descriptionReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachment->index=mp_vecAttachmentDescriptions->size()-1;
	attachment->deleteDescriptionInDestructor=false;

	mp_vecAttachments->push_back(attachment);
	mp_vecColorAttachmentRefs->push_back(attachment->descriptionReference);

	return attachment;
}

MFRenderPassAttachment* MFVkRenderPass::addDepthStencilAttachment(){
	mp_context->printInfo("MFVkRenderPass::addDepthStencilAttachment()");
	MFRenderPassAttachment* attachment=new MFRenderPassAttachment();

	VkAttachmentDescription *depthAttachment = new VkAttachmentDescription();
	if(!mp_context->getDevice()->findSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT,
			&(depthAttachment->format))){
		mp_context->printWarning("MFVkRenderPass::addDepthStencilAttachment() - no format found!");
		depthAttachment->format=VK_FORMAT_D32_SFLOAT_S8_UINT;
	}
	depthAttachment->samples = mp_context->getSetup()->m_msaaSamples;//TODO correct setup!?
	depthAttachment->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment->storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment->finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	attachment->pDescription=depthAttachment;
	attachment->deleteDescriptionInDestructor=true;
	attachment->subpassBinding=MF_VK_ATTACHMENT_DEPTH_STENCIL;
	attachment->index=mp_vecAttachmentDescriptions->size();
	attachment->descriptionReference.attachment =attachment->index;
	attachment->descriptionReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	mp_vecAttachments->push_back(attachment);
	mp_vecAttachmentDescriptions->push_back(*depthAttachment);

	return attachment;
}

MFRenderPassAttachment* MFVkRenderPass::addColorResolveAttachment(){
	mp_context->printInfo("MFVkRenderPass::addColorResolveAttachment()");
	MFRenderPassAttachment* attachment=new MFRenderPassAttachment();

	VkAttachmentDescription *colorAttachmentResolve = new VkAttachmentDescription();
	colorAttachmentResolve->format = mp_resources->getSwapchain()->getImageFormat();
	colorAttachmentResolve->samples = mp_context->getSetup()->m_msaaSamples;
	colorAttachmentResolve->loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve->storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentResolve->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentResolve->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentResolve->finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	attachment->pDescription=colorAttachmentResolve;
	attachment->deleteDescriptionInDestructor=true;
	attachment->subpassBinding=MF_VK_ATTACHMENT_RESOLVE;

	mp_vecAttachmentDescriptions->push_back(*colorAttachmentResolve);
	attachment->index=mp_vecAttachmentDescriptions->size()-1;
	attachment->descriptionReference.attachment = attachment->index;
	attachment->descriptionReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	mp_vecAttachments->push_back(attachment);
	return attachment;
}

MFVkSubpass* MFVkRenderPass::createSubpass(std::vector<MFRenderPassAttachment*>* subpassAttachments){
	mp_context->printInfo("MFVkRenderPass::createSubpass() - creating subpasses");
	MFVkSubpass* subpass = new MFVkSubpass(mp_context);
	for(MFRenderPassAttachment* attachment:(*subpassAttachments)){
		subpass->addAttachment(attachment);
	}
	subpass->createSubpass();

	mp_vecSubpasses->push_back(subpass);
	return subpass;
}

bool MFVkRenderPass::createRenderPass(bool createSubpass){
	std::vector<VkSubpassDescription> subpasses={};
	std::vector<VkSubpassDependency> dependencies={};
	for(MFVkSubpass* subpass:(*mp_vecSubpasses)){
		delete subpass;//TODO improve shitty subpass creation...
	}
	mp_vecSubpasses->clear();
	if(createSubpass){
		this->createSubpass(mp_vecAttachments);
	}
	if(mp_vecSubpasses->size()<1){
		mp_context->printErr("MFVkRenderPass::createRenderPass(bool createSubpass) - no supass was set up, can't create render pass!");
		return false;
	}
	for(MFVkSubpass* subpass:(*mp_vecSubpasses)){
		subpasses.push_back(*(subpass->getDescription()));
		dependencies.push_back(*(subpass->getDependency()));
	}
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.pNext=nullptr;
	renderPassInfo.flags=0;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(
			mp_vecAttachmentDescriptions->size());
	renderPassInfo.pAttachments = mp_vecAttachmentDescriptions->data();
	renderPassInfo.subpassCount = subpasses.size();
	renderPassInfo.pSubpasses = subpasses.data();
	renderPassInfo.dependencyCount = dependencies.size();
	renderPassInfo.pDependencies = dependencies.data();//TODO

	mp_context->printInfo(
		"MFVkRenderPass::createRenderPass()\nrenderPassInfo.dependencyCount (set to NULL!)="+
		std::to_string(renderPassInfo.dependencyCount)+
		"\nrenderPassInfo.attachmentCount="+
		std::to_string(renderPassInfo.attachmentCount)+
		"\nrenderPassInfo.subpassCount="+
		std::to_string(renderPassInfo.subpassCount));

	if (vkCreateRenderPass(*(mp_context->getVirtualDevice()),
			&renderPassInfo, nullptr, mp_renderPass) != VK_SUCCESS) {
		mp_context->printErr("MFVkRenderPass::createRenderPass() - "
				"Render pass creation failed!");
		return false;
	}
	return true;
}



