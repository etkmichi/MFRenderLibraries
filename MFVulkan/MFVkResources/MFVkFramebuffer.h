/*
 * MFVkFramebuffer.h
 *
 *  Created on: 11.10.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKRESOURCES_MFVKFRAMEBUFFER_H_
#define MFVULKAN_MFVKRESOURCES_MFVKFRAMEBUFFER_H_

class MFVkFramebuffer;

#include "vulkan/vulkan.h"
#include "MFVkResources.h"
#include "MFVkSwapchainRes.h"
#include "../MFVkContext/MFVkContext.h"
#include "../MFVkObjects/MFVkImage.h"
#include <vector>

class MFVkFramebuffer {
	MFVkResources
		*mp_resources;
	MFVkContext
		*mp_context;
	MFVkSwapchainRes
		*mp_swapchain;
	std::vector<VkFramebuffer*>
		*mp_vecFramebuffer;
	std::vector<MFVkImage*>
		*mp_vecAttachmentImages;
	std::vector<VkImageView>
		*mp_vecAttachment;

public:
	MFVkFramebuffer(MFVkContext *pContext, MFVkResources *pResources);
	virtual ~MFVkFramebuffer();

	void destroyFramebuffers();

	/**
	 * This function must be called after the swap chain was created/initialized!
	 */
	bool initFramebuffer();
	void addAttachment(MFVkImage* attachment);
	uint32_t getAttachmentCount();
	std::vector<MFVkImage*>* getVecAttachment();
	std::vector<VkFramebuffer*>* getVecFramebuffer();
	bool createFramebuffer();
};

#endif /* MFVULKAN_MFVKRESOURCES_MFVKFRAMEBUFFER_H_ */
