/*
 * MFVkRenderPass.h
 *
 *  Created on: 07.08.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKCONTEXT_MFVKRESOURCES_MFVKRENDERPASS_H_
#define MFVULKAN_MFVKCONTEXT_MFVKRESOURCES_MFVKRENDERPASS_H_

class MFVkRenderPass;
#include <vulkan/vulkan.h>
#include <vector>
#include "../MFVkResources/MFVkResources.h"
#include "../MFVkContext/MFVkContext.h"

enum MFAttachmentSubpassBinding{
	MF_VK_ATTACHMENT_INPUT,
	MF_VK_ATTACHMENT_COLOR,
	MF_VK_ATTACHMENT_RESOLVE,
	MF_VK_ATTACHMENT_DEPTH_STENCIL,
	MF_VK_ATTACHMENT_PRESERVE
};

typedef struct MFRenderPassAttachment{
	VkAttachmentDescription *pDescription;
	VkAttachmentReference descriptionReference={};
	MFAttachmentSubpassBinding subpassBinding;
	std::vector<VkSubpassDescription*> subpasses;
	uint32_t index=0xFFFFFFFF;
	bool deleteDescriptionInDestructor=false;
}MFRenderPassAttachment;

#include "../MFVkObjectComponents/MFVkSubpass.h"
/**
 * To prevent errors read alignments/constraints of the vulkan documentation!!! TODO header of lib...
 */
class MFVkRenderPass {
private:
	void*
		mp_buffer;
	void*
		mp_bufferMemory;
	MFVkContext
		*mp_context;

	MFVkResources
		*mp_resources;

	VkRenderPass
		*mp_renderPass;

	std::vector<MFVkSubpass*>
		*mp_vecSubpasses;

	std::vector<MFRenderPassAttachment*>
		*mp_vecAttachments;

	std::vector<VkAttachmentReference>
		*mp_vecInputAttachmentRefs,
		*mp_vecColorAttachmentRefs,
		*mp_vecResolveAttachmentRefs,
		*mp_vecPreserveAttachmentRefs;

	MFRenderPassAttachment
		*mp_depthStencilAttachment;

	std::vector<VkAttachmentDescription>
		*mp_vecAttachmentDescriptions;

	std::vector<VkSubpassDescription>
		*mp_vecSubpassDescriptions;

public:
	//TODO predefined adder functions - check constraints given by vulkan doc
	MFVkRenderPass(MFVkContext* pContext, MFVkResources *pResources);
	virtual ~MFVkRenderPass();
	void destroyVulkanResources();


	void cleanSetup();
	bool initRenderPass();
	MFRenderPassAttachment* addAttachment(VkAttachmentDescription &description, const MFAttachmentSubpassBinding &binding,VkImageLayout &layout);
	VkRenderPass* getRenderPass(){return mp_renderPass;};
	/**Vaihingen an der Enz
	 * Adds a predefined attachment, which is returned in the MFRenderPassAttachment structure. This attachment will be used
	 * in the initRenderPass function. It will be set up according the given setup of MFVkContext. If multisampling value is on, it will add the corresponding color resolve attachment.
	 * @return a predefined attachment.
	 */
	MFRenderPassAttachment* addColorAttachment();
//	MFRenderPassAttachment* addColorAttachment(VkAttachmentDescription colorAttachment,VkImageLayout layout);
	/**
	 * Adds a predefined attachment, which is returned in the MFRenderPassAttachment structure. This attachment will be used
	 * in the initRenderPass function.
	 * @return a predefined attachment.
	 */
	MFRenderPassAttachment* addDepthStencilAttachment();
	/*TODO setDepthStencilAttachment*/
	/*TODO setup connection with frambuffer creation and image creation of init resources*/
	/**
	 * Adds a predefined attachment, which is returned in the MFRenderPassAttachment structure. This attachment will be used
	 * in the initRenderPass function.
	 * @return a predefined attachment.
	 */
	MFRenderPassAttachment* addColorResolveAttachment();
	/**
	 * Creates a subpass with the inner field setup.
	 * @return
	 */
	MFVkSubpass* createSubpass(std::vector<MFRenderPassAttachment*>* subpassAttachments);
	//MFSubpassCompounds* createSubpass(std::vector<MFRenderPassAttachment*>);

	/**
	 * Creates the vulkan render pass. If createSubpass is false, take care to create subpass before!
	 * @param createSubpass if true it creates the subpass too. It uses all added descriptors even if they were set up faulty!
	 * @return true if successfull
	 */
	bool createRenderPass(bool createSubpass);

	std::vector<MFVkSubpass*>* getSubpasses(){return mp_vecSubpasses;};

	/**
	 *
	 * @return the attechments added to this class. The attachments are wrapped with additional data in the MFRenderPassAttachment struct.
	 */
	std::vector<MFRenderPassAttachment*>* getAttachments(){return mp_vecAttachments;};
};

#endif /* MFVULKAN_MFVKCONTEXT_MFVKRESOURCES_MFVKRENDERPASS_H_ */
