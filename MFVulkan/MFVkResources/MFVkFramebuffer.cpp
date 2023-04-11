/*
 * MFVkFramebuffer.cpp
 *
 *  Created on: 11.10.2019
 *      Author: michl
 */

#include "MFVkFramebuffer.h"

MFVkFramebuffer::MFVkFramebuffer(MFVkContext *pContext, MFVkResources *pResources) {
	mp_vecAttachmentImages = new std::vector<MFVkImage*>();
	mp_vecFramebuffer=new std::vector<VkFramebuffer*>();
	mp_vecAttachment=new std::vector<VkImageView>();
	mp_swapchain=nullptr;
	mp_context=pContext;
	mp_resources=pResources;

}

MFVkFramebuffer::~MFVkFramebuffer() {
	destroyFramebuffers();
	for(VkFramebuffer* fb: (*mp_vecFramebuffer)){
		delete fb;
	}
	mp_vecFramebuffer->clear();
	delete mp_vecFramebuffer;
}

void MFVkFramebuffer::destroyFramebuffers(){
	for(auto fb: (*mp_vecFramebuffer)){
		vkDestroyFramebuffer(*mp_context->getVirtualDevice(), *fb,nullptr);
	}
}
bool MFVkFramebuffer::initFramebuffer(){
	mp_swapchain=mp_resources->getSwapchain();
	uint32_t attCount=mp_resources->getRenderPass()->getAttachments()->size();
	if(attCount!=(mp_vecAttachmentImages->size()+1)){
		mp_context->printErr(
			"MFVkFramebuffer::initFramebuffer() - "
			"count of render pass attachment doesnt match attachment count!" +
			std::to_string(attCount)+"!="+
			std::to_string(mp_vecAttachmentImages->size()));
		return false;
	}
	if(mp_vecFramebuffer->size()!=mp_swapchain->getVecImageViews()->size()){
		uint32_t start=mp_vecFramebuffer->size();
		uint32_t stop=mp_swapchain->getVecImageViews()->size();
		mp_vecFramebuffer->resize(stop);
		for(uint32_t i=start;i<stop;i++){
			mp_vecFramebuffer->data()[i]=new VkFramebuffer();
		}
	}
	if(!createFramebuffer()){
		return false;
	}
	return true;
}
void MFVkFramebuffer::addAttachment(MFVkImage* attachment){
	mp_vecAttachmentImages->push_back(attachment);
}
uint32_t MFVkFramebuffer::getAttachmentCount(){
	return mp_vecAttachmentImages->size();
}
std::vector<MFVkImage*>* MFVkFramebuffer::getVecAttachment(){
	return mp_vecAttachmentImages;
}
std::vector<VkFramebuffer*>* MFVkFramebuffer::getVecFramebuffer(){
	return mp_vecFramebuffer;
}
bool MFVkFramebuffer::createFramebuffer(){
	for (size_t i = 0; i < mp_swapchain->getVecImageViews()->size(); i++) {
		VkFramebuffer *frameBuffer=mp_vecFramebuffer->at(i);
		mp_vecAttachment->clear();
		for(uint32_t attCounter=0;attCounter<mp_vecAttachmentImages->size();attCounter++){
			mp_vecAttachment->push_back(*(mp_vecAttachmentImages->at(attCounter)->getImageView()));
		}
		mp_vecAttachment->push_back(mp_swapchain->getVecImageViews()->at(i));
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = *(mp_resources->getRenderPass()->getRenderPass());
		framebufferInfo.width = mp_swapchain->getExtent().width;
		framebufferInfo.height = mp_swapchain->getExtent().height;
		framebufferInfo.layers = 1;
		mp_context->printInfo("MFVkFramebuffer::createFramebuffer() - "
				"framebuffer number: "+std::to_string(i));

		framebufferInfo.attachmentCount=
				static_cast<uint32_t>(mp_vecAttachment->size());
		framebufferInfo.pAttachments = mp_vecAttachment->data();

		if (vkCreateFramebuffer(
				*mp_context->getVirtualDevice(),
				&framebufferInfo,
				nullptr,
				frameBuffer) != VK_SUCCESS) {
			mp_context->printErr("MFVkFramebuffer::createFramebuffer() - Failed to create framebuffer!");
			return false;
		}
	}
	return true;
}
