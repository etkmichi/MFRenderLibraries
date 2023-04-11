/*
 * MFVkImage.h
 *
 *  Created on: 08.08.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKOBJECTS_MFVKIMAGE_H_
#define MFVULKAN_MFVKOBJECTS_MFVKIMAGE_H_

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

class MFVkImage;

#include "vulkan/vulkan.h"
#include "MFObjects/MFObject.h"
#include "../MFVkContext/MFVkContext.h"
#include "../MFVkResources/MFVkResources.h"
#include "../MFVkSceneManager/MFVkCommandManager.h"
#include "../MFVkObjects/MFVkBuffer.h"

class MFVkImage {
public:
	static MFVkImage* createDepthImage(MFVkContext *pContext, MFVkResources *pResources);
	static MFVkImage* createColorImage(MFVkContext *pContext, MFVkResources *pResources);
	static bool setupColorImage(MFVkContext *pContext, MFVkResources *pResources,MFVkImage* colorImage);
	static bool setupTextureImage(MFVkContext *pContext, MFVkResources *pResources,MFVkImage* textureImage);
	static bool setupDepthImage(MFVkContext *pContext, MFVkResources *pResources,MFVkImage* depthImage);

	/**
	 * Creates the image views of the swap chain images and sets the inner fields.
	 * @param pContext
	 * @param pResources
	 * @param swapChainImage contains the swap chain images retrieved from the vulkan api (vkGetSwapchainImagesKHR)!
	 * @return
	 */
	static bool setupSwapChainImage(MFVkContext *pContext, MFVkResources *pResources,MFVkImage* swapChainImage);
private:
	MFVkContext
		*mp_context;
	MFVkResources
		*mp_resources;

	VkFormat
		m_format;
	uint32_t
		m_width,
		m_height,
		m_mipLevels;
	int32_t
		m_channelCount,//TODO set get
		m_desiredChannels;
	uint64_t
		m_imageSize;

	VkImageViewCreateInfo
		m_viewInfo;
	VkImageCreateInfo
		m_imageInfo;

	VkImage
		m_image;
	VkDeviceMemory
		m_imageMemory;
	VkImageView
		*mp_imageView;

	VkMemoryPropertyFlags
		m_memoryProperties;

	VkSampleCountFlagBits
		m_sampleCount;
	VkImageUsageFlags
		m_usage;
	VkImageAspectFlags
		m_aspectFlags;

	VkImageTiling
		m_tiling;
	VkFormatFeatureFlags
		m_formatFeatures;

	void init();
	void createBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	void copyBufferToImage(MFVkBuffer &buffer);

	//TODO create enable disable....
public:
	MFVkImage(MFVkContext *pContext,MFVkResources *pResources);
	MFVkImage(MFVkContext *pContext,MFVkResources *pResources, VkImage& image);
	virtual ~MFVkImage();
	void destroyVulkanResources();
	void destroyImageView();
	void destroyImage();

	bool createImage();
	bool createImageView();

	VkImageViewCreateInfo& getImageViewInfo(){return m_viewInfo;};
	VkImageCreateInfo& getImageInfo(){return m_imageInfo;};

	VkImageView& createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
	/*TODO loading operation for different kind of loading an image MFTask ->MFVkStagedTexLoadTask... staged loading / direct loading...*/

	/**
	 * loads an image
	 * @return
	 */
	bool loadTextureImage(std::string filePath,int channelCount);

	/*TODO check mipmap support of image!(Depth image and some others dont use mipmaps!)*/
	bool generateMipmaps();
	VkImageView* getImageView(){return mp_imageView;};
	void setImageView(VkImageView* pImgView){mp_imageView=pImgView;};

	void setImage(VkImage& pImageHandle);
	VkImage& getImage(){return m_image;};

	void setFormat(VkFormat format);
	VkFormat& getFormat(){return m_format;};

	void setHeight(uint32_t m_height);
	uint32_t& getHeight(){return m_height;};

	void setWidth(uint32_t width);
	uint32_t& getWidth(){return m_width;};

	void setMipLevels(uint32_t mipLevels);
	uint32_t& getMipLevels(){return m_mipLevels;};

	void setSampleCountFlagBits(VkSampleCountFlagBits flagBits);//TODO enableFlag...(bool) functions
	VkSampleCountFlagBits& getSamples(){return m_sampleCount;};

	void setTiling(VkImageTiling tiling);
	VkImageTiling& getTiling(){return m_tiling;};

	void setUsage(VkImageUsageFlags usage);
	VkImageUsageFlags& getUsage(){return m_usage;};

	void setMemoryProperties(VkMemoryPropertyFlags memFlags);
	VkMemoryPropertyFlags& getMemoryProperties(){return m_memoryProperties;};

	void setDeviceMemory(VkDeviceMemory &imageMemory);//TODO imageMem is connected to image handly!
	VkDeviceMemory& getDeviceMemory(){return m_imageMemory;};

	void setAspectMask(VkImageAspectFlags aspectFlags);
	VkImageAspectFlags getAspectMask(){return m_aspectFlags;};

	void setFormatFeatures(VkFormatFeatureFlags featureFlags);
	VkFormatFeatureFlags getFormatFeatures(){return m_formatFeatures;};

	bool transferImageLayout(VkImageLayout oldLayout,VkImageLayout newLayout);

	bool checkLayoutTransition(VkImageLayout oldL,VkImageLayout newL,VkPipelineStageFlags& srcStage,VkPipelineStageFlags& dstStage,
			VkImageMemoryBarrier& barrier);

	/**
	 * Sets the VkSamplerCreateInfo needed by vulkan to create a sampler! Deletes
	 * the old one!
	 * @param createInfo
	 */
	void setSamplerCreateInfo(VkSamplerCreateInfo *createInfo);

	/**
	 *
	 * @return the sampler create info for specified setup.
	 */
//	VkSamplerCreateInfo *getSamplerCreateInfo(){return mp_createInfoSampler;};


};

#endif /* MFVULKAN_MFVKOBJECTS_MFVKIMAGE_H_ */
