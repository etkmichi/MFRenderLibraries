/*
 * MFVkResources.haaaaaa
 *
 *  Created on: 19.07.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKCONTEXT_MFVKRESOURCES_H_
#define MFVULKAN_MFVKCONTEXT_MFVKRESOURCES_H_

class MFVkResources;

#include <vulkan/vulkan.h>

#include "MFObjects/MFObject.h"
#include "../MFVkObjects/MFVkStructs.h"

#include "../MFVkObjects/MFVkImage.h"
#include "../MFVkObjects/MFVkShader.h"

#include "../MFVkSceneManager/MFVkObjectManager.h"
#include "../MFVkSceneManager/MFVkDescriptorManager.h"
#include "../MFVkSceneManager/MFVkCommandManager.h"
#include "../MFVkSceneManager/MFVkUBOManager.h"
#include "../MFVkSceneManager/MFVkShaderManager.h"

#include "../MFVkContext/MFVkContext.h"

#include "../MFVkObjectComponents/MFVkRenderPass.h"
#include "../MFVkObjectComponents/MFVkDescriptorSetLayout.h"
#include "../MFVkObjectComponents/MFVkPipeline.h"
#include "../MFVkObjectComponents/MFVkSampler.h"
#include "MFVkSwapchainRes.h"
#include "MFVkFramebuffer.h"
#include "../MFVkFactories/MFVkDescriptorLayoutFactory.h"
#include "../MFPredefinedObjectSetups/MFAbstractSetup.h"

#include "../../MFRenderResources.h"

/**
 * This class creates, gathers and initialices resources for running vulkan. If implemented,
 * it provides access to these resources for advanced options.
 */
class MFVkResources : public MFRenderResources{
private:
	MFVkContext
		*mp_context;

	MFVkSwapchainRes
		*mp_swapChain;
	MFVkRenderPass
		*mp_renderPass;

	MFVkFramebuffer
		*mp_framebuffer;

	MFVkDescriptorManager
		*mp_descriptorManager;

	MFVkCommandManager
		*mp_commandManager;

	MFVkPipelineManager
		*mp_pipelineManager;

	MFVkObjectManager
		*mp_objectManager;

	MFVkShaderManager
		*mp_shaderManager;

	MFVkImage
		*mp_colorImage,
		*mp_depthImage;

	MFVkSampler
		*mp_sampler;

	std::vector<MFVkImage>
		*mp_vecImages;

	std::vector<MFAbstractSetup*>
		*mp_vecObjectSetups;

	bool
		m_isInitialized=false;

	MFAbstractSetup
		*mp_defaultSetup;


	/*TODO store used queue families in resources - context provices all queue familiy indices*/
public:
	MFVkResources(MFVkContext* pContext);
	virtual ~MFVkResources();
	MFAbstractSetup* getDefaultObjectSetup(){return mp_defaultSetup;};
	std::vector<MFAbstractSetup*>* getVecObjectSetups(){return mp_vecObjectSetups;};

	void addVertexShader(MFVkShader* pVertexShader);
	void addFragmentShader(MFVkShader* pFragmentShader);

	bool initResources();
	bool isInitialized(){return m_isInitialized;};
	bool recreateSwapChain();
	void cleanupSwapChain();

	bool initSwapChain_1();

	bool initRenderPass_2();

	bool initDescriptorSetups_3();
	bool initShader_3_1();

	bool initPipelineManager_4();
	bool initPipelineSetups_4_1();

	/*Can be init after device init!*/
	bool initCommandManager_5();

	bool initColorImage_6();
	bool initUBOManager_6_1();
	bool initObjectManager_6_2();

	bool initDepthImage_7(bool recreate);

	bool initFramebuffers_8();

	/**
	 * Creates the sampler for a texture. To setup the sampler use the returned value of getSamplerCreateInfo() or
	 * set an own create info with setSamplerCreateInfo(...).
	 * @return true if successful
	 */
	bool initSampler_9();

	VkSampler* getTextureSampler();
	MFVkSwapchainRes* getSwapchain(){return mp_swapChain;};
	MFVkFramebuffer* getFramebuffer(){return mp_framebuffer;};

	MFVkCommandManager* getCommandManager(){return mp_commandManager;};
	MFVkDescriptorManager* getDescriptorManager(){return mp_descriptorManager;};
	MFVkPipelineManager* getPipelineManager(){return mp_pipelineManager;};
	MFVkObjectManager* getObjectManager(){return mp_objectManager;};
	MFVkShaderManager* getShaderManager(){return mp_shaderManager;};

	VkExtent2D getSwapChainExtent();

	/*functions which may be out sourced*/
	VkImageView createImageView(
			VkImage &image,
			VkFormat format,
			VkImageAspectFlags aspectFlags,
			uint32_t mipLevels);

	VkFormat findSupportedFormat(
			const std::vector<VkFormat>& candidates,
			VkImageTiling tiling,
			VkFormatFeatureFlags features);

	MFVkImage* getColorImage();
	MFVkImage* getDepthImage();

	bool createColorResources();/*TODO complete class MFVkImage with createImage -> create a local field as MFVkImage colorImages*/
	bool createDepthResources();

	void addObjectSetup(MFAbstractSetup* pSetup);
	MFVkRenderPass* getRenderPass(){return mp_renderPass;};
};

#endif /* MFVULKAN_MFVKCONTEXT_MFVKRESOURCES_H_ */
