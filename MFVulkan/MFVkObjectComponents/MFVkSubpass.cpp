/*
 * MFVkSubpass.cpp
 *
 *  Created on: 12.08.2019
 *      Author: michl
 */

#include "MFVkSubpass.h"

MFVkSubpass::MFVkSubpass(MFVkContext* pContext) {
	mp_context = pContext;
	mp_vecInputAttachmentRefs = new std::vector<VkAttachmentReference>();

	mp_vecColorAttachmentRefs = new std::vector<VkAttachmentReference>();
	mp_vecResolveAttachmentRefs = new std::vector<VkAttachmentReference>();
	mp_vecPreserveAttachmentRefs = new std::vector<uint32_t>();

	mp_subpassDescription = new VkSubpassDescription();
	mp_subpassDependency = new VkSubpassDependency();

	mp_depthStencilAttachmentRef = nullptr;
	mp_vecAttachments = new std::vector<MFRenderPassAttachment*>();
}

MFVkSubpass::~MFVkSubpass() {
	delete mp_vecInputAttachmentRefs;
	delete mp_vecColorAttachmentRefs;
	delete mp_vecResolveAttachmentRefs;
	delete mp_vecPreserveAttachmentRefs;
	delete mp_subpassDescription;
	delete mp_subpassDependency;
	delete mp_vecAttachments;
}

void MFVkSubpass::clearAttachments(){
	mp_vecInputAttachmentRefs->clear();
	mp_vecColorAttachmentRefs->clear();
	mp_vecResolveAttachmentRefs->clear();
	mp_vecPreserveAttachmentRefs->clear();
	mp_vecAttachments->clear();
	mp_depthStencilAttachmentRef=nullptr;
}

bool MFVkSubpass::createSubpass(){
	bool ret=true;
	mp_context->printInfo("MFVkSubpass::createSubpass() - filling subpass description");
	mp_subpassDescription->pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	mp_subpassDescription->flags = 0;
	mp_subpassDescription->inputAttachmentCount = mp_vecInputAttachmentRefs->size();
	mp_subpassDescription->pInputAttachments = mp_vecInputAttachmentRefs->data();
	mp_subpassDescription->colorAttachmentCount = mp_vecColorAttachmentRefs->size();
	mp_subpassDescription->pColorAttachments = mp_vecColorAttachmentRefs->data();
	mp_subpassDescription->pResolveAttachments = mp_vecResolveAttachmentRefs->data();
	mp_subpassDescription->preserveAttachmentCount = mp_vecPreserveAttachmentRefs->size();
	mp_subpassDescription->pPreserveAttachments = mp_vecPreserveAttachmentRefs->data();
	mp_subpassDescription->pDepthStencilAttachment = mp_depthStencilAttachmentRef;

	std::string info="";
	info+="\tInput attachment count: "+std::to_string(mp_subpassDescription->inputAttachmentCount)+"\n";
	info+="\tColor attachment count: "+std::to_string(mp_subpassDescription->colorAttachmentCount)+"\n";
	info+="\tResolve attachment count: "+std::to_string(mp_vecResolveAttachmentRefs->size())+"\n";
	info+="\tPreserve attachment count: "+std::to_string(mp_vecPreserveAttachmentRefs->size())+"\n";
	info+="\tStencil attachment pointer: "+std::to_string((long)(mp_depthStencilAttachmentRef))+"\n";
	mp_context->printInfo("MFVkSubpass::createSubpass() - attachment setup: \n"+info);

	mp_subpassDependency->srcSubpass = VK_SUBPASS_EXTERNAL;
	mp_subpassDependency->dstSubpass = 0;
	mp_subpassDependency->srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;//COLOR_ATTACHMENT_OUTPUT_BIT;
	mp_subpassDependency->srcAccessMask = 0;
	mp_subpassDependency->dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;//VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	mp_subpassDependency->dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	return ret;
}

void MFVkSubpass::addAttachment(MFRenderPassAttachment *pAttachment){
	mp_vecAttachments->push_back(pAttachment);
		switch(pAttachment->subpassBinding){
		case MF_VK_ATTACHMENT_COLOR:
			mp_context->printInfo("MFVkSubpass::addAttachment() - MF_VK_ATTACHMENT_COLOR");
			mp_vecColorAttachmentRefs->push_back(pAttachment->descriptionReference);
			break;
		case MF_VK_ATTACHMENT_DEPTH_STENCIL:
			mp_context->printInfo("MFVkSubpass::addAttachment() - MF_VK_ATTACHMENT_DEPTH_STENCIL");
			mp_depthStencilAttachmentRef = &(pAttachment->descriptionReference);
			break;
		case MF_VK_ATTACHMENT_INPUT:
			mp_context->printInfo("MFVkSubpass::addAttachment() - MF_VK_ATTACHMENT_INPUT");
			mp_vecInputAttachmentRefs->push_back(pAttachment->descriptionReference);
			break;
		case MF_VK_ATTACHMENT_PRESERVE:
			mp_context->printInfo("MFVkSubpass::addAttachment() - MF_VK_ATTACHMENT_PRESERVE");
			mp_context->printInfo("No preserve attachment yet!");
			break;
		case MF_VK_ATTACHMENT_RESOLVE:
			mp_context->printInfo("subpass - adding MF_VK_ATTACHMENT_RESOLVE");
			mp_vecResolveAttachmentRefs->push_back(pAttachment->descriptionReference);
			break;
		default:
			mp_context->printErr("Couldn't find match for attachment binding type!");
		}
}

void MFVkSubpass::setDepthStencilAttachment(VkAttachmentReference *depthStencilAttachment){
	mp_depthStencilAttachmentRef=depthStencilAttachment;
}
