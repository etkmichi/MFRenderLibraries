/*
 * MFVkSwapChain.h
 *
 *  Created on: 06.08.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKCONTEXT_MFVKRESOURCES_MFVKSWAPCHAINRES_H_
#define MFVULKAN_MFVKCONTEXT_MFVKRESOURCES_MFVKSWAPCHAINRES_H_

class MFVkSwapchainRes;

#include "vulkan/vulkan.h"
#include <MFObjects/MFObject.h>
#include "MFVkResources.h"
#include "../MFVkObjects/MFVkImage.h"
#include "../MFVkSceneManager/MFVkPipelineManager.h"

class MFVkContext;

class MFVkSwapchainRes {
private:
	MFVkContext
		*mp_context;

	MFVkResources
		*mp_resources;

	VkSurfaceCapabilitiesKHR
		*mp_surfaceCapabilities;

	std::vector<VkSurfaceFormatKHR>
		*mp_surfaceFormats;

	VkSurfaceFormatKHR
		*mp_surfaceFormat;

	std::vector<VkPresentModeKHR>
		*mp_presentModes;
	VkPresentModeKHR
		*mp_presentMode;

	VkExtent2D
		m_swapChainExtent;

	VkFormat
		m_swapChainImageFormat;

	uint32_t
		m_swapChainMinImageCount=2,
		m_swapChainImageCount=2;

	VkSwapchainCreateInfoKHR
		*mp_swapChainCreateInfo;

	VkSwapchainKHR
		*mp_swapChain;

	std::vector<VkImage>
		*mp_vecSwapChainImage;
	std::vector<VkImageView>
		*mp_vecSwapChainImageView;
	std::vector<MFVkImage*>
		*mp_vecWrappedImage;

	bool m_fullInit;

	bool initResources();
	void initSurfaceFormat();
	void initPresentMode();
	void initExtent();

	/*This should be called after format was set!*/
	bool initImageViews();
public:
	MFVkSwapchainRes(MFVkContext* pContext, MFVkResources* pResources);
	virtual ~MFVkSwapchainRes();
	void destroyImages();
	void destroyImageViews();
	void destroySwapChain();
	bool initSwapChain();

	VkSwapchainKHR* getSwapChain(){return mp_swapChain;};
	uint32_t getImageCount(){return m_swapChainImageCount;};
	VkFormat getImageFormat(){return m_swapChainImageFormat;};
	VkExtent2D getExtent(){return m_swapChainExtent;};
	std::vector<VkImage>* getVecImages(){return mp_vecSwapChainImage;};
	std::vector<VkImageView>* getVecImageViews(){return mp_vecSwapChainImageView;};
	std::vector<MFVkImage*>* getVecWrappedImages(){return mp_vecWrappedImage;};

};



#endif /* MFVULKAN_MFVKCONTEXT_MFVKRESOURCES_MFVKSWAPCHAINRES_H_ */
