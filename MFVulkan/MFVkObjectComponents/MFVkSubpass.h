/*
 * MFVkSubpass.h
 *
 *  Created on: 12.08.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKCONTEXT_MFVKRESOURCES_MFVKSUBPASS_H_
#define MFVULKAN_MFVKCONTEXT_MFVKRESOURCES_MFVKSUBPASS_H_

class MFVkSubpass;

#include <vector>
#include "../MFVkObjectComponents/MFVkRenderPass.h"
#include "vulkan/vulkan.h"
#include "../MFVkContext/MFVkContext.h"
class MFVkRenderPass;

//typedef struct MFSubpassCompounds{
//	VkSubpassDescription *subpass;
//	VkSubpassDependency *dependency;
//	bool deleteInDestructor=false;
//};

class MFVkSubpass {
public:
	MFVkContext
		*mp_context;

	VkSubpassDependency
		*mp_subpassDependency;
	VkSubpassDescription
		*mp_subpassDescription;

	std::vector<VkAttachmentReference>
		*mp_vecInputAttachmentRefs,
		*mp_vecColorAttachmentRefs,
		*mp_vecResolveAttachmentRefs;
	std::vector<uint32_t>
		*mp_vecPreserveAttachmentRefs;

	VkAttachmentReference
		*mp_depthStencilAttachmentRef;

	std::vector<MFRenderPassAttachment*>
		*mp_vecAttachments;

	MFVkSubpass(MFVkContext* pContext);

	virtual ~MFVkSubpass();

	void clearAttachments();

	bool createSubpass();

	/**
	 * @param pAttachment
	 */
	void addAttachment(MFRenderPassAttachment* pAttachment);

	void setDepthStencilAttachment(VkAttachmentReference *depthStencilAttachment);

	VkSubpassDescription* getDescription(){return mp_subpassDescription;};
	VkSubpassDependency* getDependency(){return mp_subpassDependency;};
};

#endif /* MFVULKAN_MFVKCONTEXT_MFVKRESOURCES_MFVKSUBPASS_H_ */
