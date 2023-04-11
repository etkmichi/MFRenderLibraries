/*
 * MFVkImage.cpp
 *
 *  Created on: 08.08.2019
 *      Author: michl
 */

#include "MFVkImage.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
MFVkImage::MFVkImage(MFVkContext *pContext,MFVkResources *pResources){
	mp_context=pContext;
	mp_resources = pResources;

	m_image=VkImage();
	init();

	m_imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	m_imageInfo.imageType = VK_IMAGE_TYPE_2D;
	m_imageInfo.extent.width = m_width;
	m_imageInfo.extent.height = m_height;
	m_imageInfo.extent.depth = 1;
	m_imageInfo.mipLevels = m_mipLevels;
	m_imageInfo.arrayLayers = 1;
	m_imageInfo.format = m_format;
	m_imageInfo.tiling = m_tiling;
	m_imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	m_imageInfo.usage = m_usage;
	m_imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;//mp_context->getSetup()->m_msaaSamples;
	m_imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	m_imageInfo.samples = m_sampleCount;

}

MFVkImage::MFVkImage(MFVkContext *pContext,MFVkResources *pResources,VkImage &image){
	mp_context=pContext;
	mp_resources = pResources;
	m_image=image;
	init();
}

void MFVkImage::init(){
	m_width=600;
	m_height=800;
	m_mipLevels=1;
	m_channelCount=4;/*RGBA*/
	m_desiredChannels=4;
	m_imageSize=m_width*m_height*m_channelCount;
	m_format=VK_FORMAT_B8G8R8A8_UNORM;

	m_viewInfo = {};

	m_imageInfo = {};
//
	m_format=VkFormat();
	m_sampleCount=VK_SAMPLE_COUNT_1_BIT;//mp_context->getSetup()->m_msaaSamples;
	m_tiling=VK_IMAGE_TILING_OPTIMAL;
	m_usage=0;//VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT
	m_memoryProperties=VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	m_imageMemory=VkDeviceMemory();
	mp_imageView=new VkImageView();
	m_aspectFlags=(VK_IMAGE_ASPECT_COLOR_BIT);
	m_formatFeatures=0;

	m_viewInfo = {};
	m_viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	m_viewInfo.image = m_image;
	m_viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	m_viewInfo.format = m_format;
	m_viewInfo.subresourceRange.aspectMask = m_aspectFlags;
	m_viewInfo.subresourceRange.baseMipLevel = 0;
	m_viewInfo.subresourceRange.levelCount = m_mipLevels;
	m_viewInfo.subresourceRange.baseArrayLayer = 0;
	m_viewInfo.subresourceRange.layerCount = 1;
	m_viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;


	m_imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	m_imageInfo.imageType = VK_IMAGE_TYPE_2D;
	m_imageInfo.extent.width = m_width;
	m_imageInfo.extent.height = m_height;
	m_imageInfo.extent.depth = 1;
	m_imageInfo.mipLevels = m_mipLevels;
	m_imageInfo.arrayLayers = 1;
	m_imageInfo.format = m_format;
	m_imageInfo.tiling = m_tiling;
	m_imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	m_imageInfo.usage = m_usage;
	m_imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;//mp_context->getSetup()->m_msaaSamples;
	m_imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	m_imageInfo.samples = m_sampleCount;
}

MFVkImage::~MFVkImage() {
	/*TODO what happens if image was not created=?*/
	destroyVulkanResources();
}

void MFVkImage::destroyVulkanResources(){
	destroyImageView();
	destroyImage();
}

void MFVkImage::destroyImageView(){
	vkDestroyImageView(*(mp_context->getVirtualDevice()), *mp_imageView,nullptr);
}
void MFVkImage::destroyImage(){
	if(m_image!=VK_NULL_HANDLE){
		vkDestroyImage(*(mp_context->getVirtualDevice()),m_image,nullptr);
		vkFreeMemory(*(mp_context->getVirtualDevice()), m_imageMemory,nullptr);
	}else{
		mp_context->printWarning("MFVkImage::destroyImage() - VkImage is nullhandle!");
	}
}

MFVkImage* MFVkImage::createDepthImage(MFVkContext *pContext, MFVkResources *pResources){
	//MFVkImage *depthImage=new MFVkImage();

	return nullptr;
}

MFVkImage* MFVkImage::createColorImage(MFVkContext *pContext, MFVkResources *pResources){
	MFVkImage *colorImage=new MFVkImage(pContext,pResources);
	colorImage->setHeight(pResources->getSwapchain()->getExtent().height);
	colorImage->setWidth(pResources->getSwapchain()->getExtent().width);
	colorImage->setMipLevels(1);
	colorImage->setSampleCountFlagBits(pContext->getSetup()->m_msaaSamples);//pContext->getSetup()->m_msaaSamples);
	colorImage->setFormat(pResources->getSwapchain()->getImageFormat());
	colorImage->setTiling(VK_IMAGE_TILING_OPTIMAL);
	colorImage->setUsage(VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
	colorImage->setAspectMask(VK_IMAGE_ASPECT_COLOR_BIT);
	return colorImage;
}

bool MFVkImage::setupColorImage(MFVkContext *pContext, MFVkResources *pResources,MFVkImage* colorImage){
  pContext->printInfo("MFVkImage - setup color image");
	if(!pResources->isInitialized()){
	  pContext->printInfo("MFVkImage - setup color image - swap chain not initialized!");
		return false;
	}
	colorImage->setMemoryProperties(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	colorImage->setHeight(pResources->getSwapchain()->getExtent().height);
	colorImage->setWidth(pResources->getSwapchain()->getExtent().width);
	colorImage->setMipLevels(1);
	colorImage->setSampleCountFlagBits(pContext->getSetup()->m_msaaSamples);
	colorImage->setFormat(pResources->getSwapchain()->getImageFormat());
	colorImage->setTiling(VK_IMAGE_TILING_OPTIMAL);
	colorImage->setUsage(VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
	colorImage->setAspectMask(VK_IMAGE_ASPECT_COLOR_BIT);
	return true;
}
bool MFVkImage::setupDepthImage(MFVkContext *pContext, MFVkResources *pResources,MFVkImage* depthImage){
	depthImage->setHeight(pResources->getSwapchain()->getExtent().height);
	depthImage->setWidth(pResources->getSwapchain()->getExtent().width);
	depthImage->setMipLevels(1);
	depthImage->setSampleCountFlagBits(pContext->getSetup()->m_msaaSamples);
	depthImage->setFormatFeatures(VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	depthImage->setTiling(VK_IMAGE_TILING_OPTIMAL);
	depthImage->setUsage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
	depthImage->setAspectMask(VK_IMAGE_ASPECT_DEPTH_BIT);
	depthImage->setMemoryProperties(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VkFormat format;
	if(!pContext->getDevice()->findSupportedFormat(
		 { VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT,
		&format)){
		return false;
	}
	depthImage->setFormat(format);

	return true;
}

bool MFVkImage::setupSwapChainImage(MFVkContext *pContext,
		MFVkResources *pResources,MFVkImage* swapChainImage){
	swapChainImage->setAspectMask(VK_IMAGE_ASPECT_COLOR_BIT);
	swapChainImage->setUsage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
	swapChainImage->setMipLevels(1);
	swapChainImage->setFormat(pResources->getSwapchain()->getImageFormat());
	swapChainImage->setSampleCountFlagBits(VK_SAMPLE_COUNT_1_BIT);
	swapChainImage->getImageViewInfo().components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	swapChainImage->getImageViewInfo().components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	swapChainImage->getImageViewInfo().components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	swapChainImage->getImageViewInfo().components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	swapChainImage->getImageViewInfo().subresourceRange.baseMipLevel = 0;
	swapChainImage->getImageViewInfo().subresourceRange.levelCount=1;
	swapChainImage->getImageViewInfo().subresourceRange.baseArrayLayer=0;
	swapChainImage->getImageViewInfo().subresourceRange.layerCount=1;
	swapChainImage->getImageViewInfo().subresourceRange.aspectMask=VK_IMAGE_ASPECT_COLOR_BIT;
	return true;
}

bool MFVkImage::setupTextureImage(MFVkContext *pContext,
		MFVkResources *pResources,MFVkImage* textureImage){
	textureImage->setAspectMask(VK_IMAGE_ASPECT_COLOR_BIT);
	textureImage->setMemoryProperties(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	textureImage->setUsage(VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
	textureImage->setTiling(VK_IMAGE_TILING_OPTIMAL);
	textureImage->setFormat(VK_FORMAT_R8G8B8A8_UNORM);
	textureImage->setSampleCountFlagBits(VK_SAMPLE_COUNT_1_BIT);
	return true;
}

/*TODO enable mip levels...*/
bool MFVkImage::loadTextureImage(std::string filePath,int channelCount){
	bool ret=true;
	mp_context->printInfo("Loading texture image!");
	//int texWidth,texHeight,texChannels;
	m_desiredChannels = channelCount;

	int width,height;
	/*TODO test if uint is ok with m_width and height!*/
	stbi_uc* pixels = stbi_load(filePath.c_str(),&width,&height,&m_channelCount,channelCount);
	setWidth(static_cast<uint32_t>(width));
	setHeight(static_cast<uint32_t>(height));
	VkDeviceSize imageSize = m_width * m_height * m_desiredChannels;
	m_mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(m_width, m_height)))) + 1;
	setMipLevels(m_mipLevels);
	if (!pixels) {
		mp_context->printErr("failed to load texture image!");
		return false;
	}
	MFVkBuffer buffer(mp_context,mp_resources,imageSize);
	buffer.setUsage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	buffer.setMemoryProperties(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	if(!buffer.createBuffer())return false;
	buffer.mapData(pixels);

	stbi_image_free(pixels);

	/*todo loadTex---> static setupTextureImage(... ,MFVkImage texImg){...*/
	m_format=VK_FORMAT_R8G8B8A8_UNORM;/*TODO dependend on img source??*/
	m_usage=VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

	createImage();

	transferImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	copyBufferToImage(buffer);

	if(!generateMipmaps())return false;
	if(!createImageView())return false;
	return ret;
}

bool MFVkImage::generateMipmaps(){
	if(!mp_context->getDevice()->checkFormatPropertyOptimalTiling(m_format,VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))return false;

	VkCommandBuffer* commandBuffer = mp_resources->getCommandManager()->beginSingleTimeCommands();
	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image = m_image;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = m_aspectFlags;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.levelCount = 1;

	int32_t mipWidth = m_width;
	int32_t mipHeight = m_height;
	if(mipWidth < 0 || mipHeight < 0){
		mp_context->printErr("mip width || mip height < 0! w h"+std::to_string(mipWidth)+" "+std::to_string(mipHeight));
		return false;

	}
	for (uint32_t i = 1; i < m_mipLevels; i++) {
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(*commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		VkImageBlit blit = {};
		blit.srcOffsets[0] = {0, 0, 0};
		blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		blit.dstOffsets[0] = {0, 0, 0};
		blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		vkCmdBlitImage(*commandBuffer,
			m_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &blit,
			VK_FILTER_LINEAR);

		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(*commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		if (mipWidth > 1) mipWidth /= 2;
		if (mipHeight > 1) mipHeight /= 2;
	}
	barrier.subresourceRange.baseMipLevel = m_mipLevels - 1;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(*commandBuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
		0, nullptr,
		0, nullptr,
		1, &barrier);

	mp_resources->getCommandManager()->endSingleTimeCommands(commandBuffer);
	return true;
}

void MFVkImage::copyBufferToImage(MFVkBuffer &buffer){
	VkCommandBuffer* commandBuffer = mp_resources->getCommandManager()->beginSingleTimeCommands();
	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = m_aspectFlags;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = {0, 0, 0};
	region.imageExtent = {
		m_width,
		m_height,
		1
	};

	vkCmdCopyBufferToImage(*commandBuffer, *(buffer.getBuffer()), m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	mp_resources->getCommandManager()->endSingleTimeCommands(commandBuffer);
}


void MFVkImage::setFormatFeatures(VkFormatFeatureFlags featureFlags){
	m_formatFeatures=featureFlags;
}


bool MFVkImage::createImage(){
	mp_context->printInfo("MFVkImage - creating image");
	if (vkCreateImage(*(mp_context->getVirtualDevice()), &m_imageInfo, nullptr, &m_image) != VK_SUCCESS) {
		mp_context->printErr("failed to create VkImage!");
		return false;
	}
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(*(mp_context->getVirtualDevice()), m_image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	if(! mp_context->getDevice()->getMemoryIndex(allocInfo.memoryTypeIndex,&memRequirements, m_memoryProperties)){
		mp_context->printErr("MFVkImage - creating image - no memory found!");
		return false;
	}

	if (vkAllocateMemory(*mp_context->getVirtualDevice(), &allocInfo, nullptr, &m_imageMemory) != VK_SUCCESS) {
		mp_context->printErr("failed to allocate image memory!");
		return false;
	}

	vkBindImageMemory(*mp_context->getVirtualDevice(), m_image, m_imageMemory, 0);
	m_viewInfo.image=m_image;
	mp_context->printInfo("MFVkImage - creating image - done");
	return true;
}

bool MFVkImage::createImageView(){
	mp_context->printInfo("MFVkImage::createImageView()");
	if (vkCreateImageView(*(mp_context->getVirtualDevice()), &m_viewInfo, nullptr,mp_imageView) != VK_SUCCESS) {
		mp_context->printErr("MFVkImage::createImageView() - Failed to create image view!");
		return false;
	}
	return true;
}

bool MFVkImage::transferImageLayout(VkImageLayout oldLayout,VkImageLayout newLayout){
	mp_context->printInfo("MFVkImage - transfering image layout");
	VkCommandBuffer* commandBuffer = mp_resources->getCommandManager()->beginSingleTimeCommands();
	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = m_image;
	barrier.subresourceRange.aspectMask=m_aspectFlags;
	if(m_format==VK_FORMAT_D32_SFLOAT_S8_UINT || m_format == VK_FORMAT_D24_UNORM_S8_UINT){/*with stencil*/
		barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = m_mipLevels;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if(!checkLayoutTransition(oldLayout,newLayout,sourceStage,destinationStage,barrier))
		return false;

	vkCmdPipelineBarrier(
			*commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr, //Memory barrier
			0,nullptr, //buffer barrier
			1, &barrier);//image barrier
	mp_context->printInfo("MFVkImage - transferImageLayout done!");
	mp_resources->getCommandManager()->endSingleTimeCommands(commandBuffer);
	return true;
}

void MFVkImage::setImage(VkImage& imageHandle){
	m_image=imageHandle;
	m_viewInfo.image=m_image;
	//m_viewInfo.format = m_format;
}
void MFVkImage::setFormat(VkFormat format){
	m_format=format;
	m_imageInfo.format = m_format;
	m_viewInfo.format = m_format;
}
void MFVkImage::setHeight(uint32_t height){
	mp_context->printInfo("MFVkImage::setHeight - to:"+std::to_string(height));
	m_height=height;
	m_imageSize=m_width*m_height*m_channelCount;
	m_imageInfo.extent.height = height;
}
void MFVkImage::setWidth(uint32_t width){
	mp_context->printInfo("MFVkImage::setWidth - to:"+std::to_string(width));
	m_width=width;
	m_imageSize=m_width*m_height*m_channelCount;
	m_imageInfo.extent.width = width;
}

void MFVkImage::setMipLevels(uint32_t mipLevels){
	m_mipLevels=mipLevels;
	m_imageInfo.mipLevels = m_mipLevels;
	m_viewInfo.subresourceRange.levelCount = m_mipLevels;
	//mp_createInfoSampler->maxLod = static_cast<float>(m_mipLevels);
}

void MFVkImage::setSampleCountFlagBits(VkSampleCountFlagBits flagBits){
	m_sampleCount=flagBits;
	m_imageInfo.samples = m_sampleCount;
}
void MFVkImage::setTiling(VkImageTiling tiling){
	m_tiling=tiling;
	m_imageInfo.tiling = m_tiling;
}
void MFVkImage::setUsage(VkImageUsageFlags usage){
	m_usage=usage;
	m_imageInfo.usage = m_usage;
}
void MFVkImage::setMemoryProperties(VkMemoryPropertyFlags memFlags){
	m_memoryProperties=memFlags;
}
void MFVkImage::setDeviceMemory(VkDeviceMemory& imageMemory){
	m_imageMemory=imageMemory;
}
void MFVkImage::setAspectMask(VkImageAspectFlags aspectFlags){
	m_aspectFlags=aspectFlags;
	m_viewInfo.subresourceRange.aspectMask = m_aspectFlags;
}

bool MFVkImage::checkLayoutTransition(VkImageLayout oldLayout,
		VkImageLayout newLayout,
		VkPipelineStageFlags& sourceStage,
		VkPipelineStageFlags& destinationStage,
		VkImageMemoryBarrier& barrier){
	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {//new transition
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	} else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {//for staging buffer?
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	} else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}else if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL){
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}
	else {
		mp_context->printErr("MFVkImage::checkLayoutTransition - unsupported layout transition!");
		return false;
	}
	return true;
//	switch(newLayout){
//		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
//			if(oldLayout==VK_IMAGE_LAYOUT_UNDEFINED){
//				barrier.srcAccessMask = 0;
//				barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//				sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//				destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//				break;
//			}
//			mp_context->printErr("cant transfer from old layout!=VK_IMAGE_LAYOUT_UNDEFINED to newLayout VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL | Old layout:"+std::to_string(oldLayout));
//			return false;
//		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
//			if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL){
//				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//				sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//				destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//				break;
//			}
//			mp_context->printErr("cant transfer from old layout!=VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL to newLayout VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL | Old layout:"+std::to_string(oldLayout));
//			return false;
//		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
//			if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED) {
//				barrier.srcAccessMask = 0;
//				barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
//				sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//				destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
//				break;
//			}
//			mp_context->printErr("cant transfer from old layout!=VK_IMAGE_LAYOUT_UNDEFINED to newLayout VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL | Old layout:"+std::to_string(oldLayout));
//			return false;
//		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
//			if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED){
//				barrier.srcAccessMask = 0;
//				barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//				sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//				destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//				break;
//			}
//			mp_context->printErr("cant transfer from old layout!=VK_IMAGE_LAYOUT_UNDEFINED to newLayout==VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL | Old layout:"+std::to_string(oldLayout));
//			return false;
//		default:
//			return false;
//		}
//
//
//		return true;
}
//void MFVkImage::setSamplerCreateInfo(VkSamplerCreateInfo* createInfo){
//	if(createInfo!=nullptr){
//		if(mp_createInfoSampler!=nullptr){
//			mp_context->printInfo("MFVkImage - deleting old sampler create info!");
//			delete mp_createInfoSampler;
//		}
//		mp_createInfoSampler=createInfo;
//	}
//}

