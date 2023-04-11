#include "MFVkSwapchainRes.h"

/*
 * MFVkSwapchainRes.cpp
 *
 *  Created on: 18.07.2019
 *      Author: michl
 */


MFVkSwapchainRes::MFVkSwapchainRes(MFVkContext* pContext, MFVkResources* pResources):
m_swapChainImageFormat()
{
	mp_context = pContext;
	mp_resources = pResources;
	mp_surfaceCapabilities = new VkSurfaceCapabilitiesKHR();
	mp_presentMode=new VkPresentModeKHR();
	mp_presentModes=new std::vector<VkPresentModeKHR>();
	mp_surfaceFormats = new std::vector<VkSurfaceFormatKHR>();
	mp_surfaceFormat = new VkSurfaceFormatKHR();
	m_swapChainMinImageCount=mp_context->getSetup()->m_swapChainMinImageCount;//changed during init to the driver setup value
	m_fullInit=true;
	mp_swapChainCreateInfo=new VkSwapchainCreateInfoKHR();
	mp_swapChain=new VkSwapchainKHR;

	mp_vecWrappedImage= new std::vector<MFVkImage*>();
	mp_vecSwapChainImage = new std::vector<VkImage>();
	mp_vecSwapChainImageView = new std::vector<VkImageView>();

	m_swapChainExtent={};
//	m_swapChainExtent.width=400;//TODO
//	m_swapChainExtent.height=300;
}

MFVkSwapchainRes::~MFVkSwapchainRes() {
	vkDestroySwapchainKHR(*mp_context->getVirtualDevice(), *mp_swapChain, nullptr);
	delete mp_surfaceCapabilities;
	delete mp_presentMode;
	delete mp_presentModes;
	delete mp_surfaceFormats;
	delete mp_surfaceFormat;
	delete mp_swapChainCreateInfo;


	/*todo delete pointer in vector!*/
	delete mp_vecSwapChainImage;

	destroyImages();
	delete mp_vecWrappedImage;

	destroySwapChain();
	delete mp_swapChain;
}

void MFVkSwapchainRes::destroyImages(){
	mp_context->printWarning("MFVkSwapchainRes::destroyImages() - "
			"This will delete the image too, which wasnt initialized! May not work!");
	for(MFVkImage *img : (*mp_vecWrappedImage)){
		if(img!=nullptr)
			img->destroyImage();/*No image, just image view*/
	}
}
void MFVkSwapchainRes::destroyImageViews(){
	for(MFVkImage *img : (*mp_vecWrappedImage)){
		if(img!=nullptr)
			img->destroyImageView();/*No image, just image view*/
	}
}
void MFVkSwapchainRes::destroySwapChain(){
	if(mp_swapChain!=nullptr && mp_context!=nullptr){
	    vkDestroySwapchainKHR(*mp_context->getVirtualDevice(), *mp_swapChain, nullptr);
	}else{
		mp_context->printErr("MFVkSwapchainRes::destroySwapChain() - "
				"mp_swapChain==nullptr || mp_context==nullptr!");
	}
}

bool MFVkSwapchainRes::initSwapChain(){
	bool ret=true;
	std::string swapChainInfo="MFVkSwapchainRes::initSwapChain - init info's:\n";
	if(!initResources())
		return false;
	initSurfaceFormat();
	initPresentMode();
	initExtent();

	m_swapChainMinImageCount=mp_context->getSetup()->m_swapChainMinImageCount;
	if( m_swapChainMinImageCount < mp_surfaceCapabilities->minImageCount){
		mp_context->printWarning("MFVkSwapchainRes::initSwapChain - "
				"couldn't setup swap chain image count with setup value, "
				"#because it is lower than the min value! "+
				std::to_string(m_swapChainMinImageCount)+"<"+std::to_string(mp_surfaceCapabilities->minImageCount));
		m_swapChainMinImageCount=mp_surfaceCapabilities->minImageCount;
	}
	swapChainInfo+=(("MFVkSwapchainRes::initSwapChain - swap chain min image count: "+
			std::to_string(m_swapChainMinImageCount))+"\n");

	mp_swapChainCreateInfo->sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

	if(mp_context->getDevice()->getGraphicsQueueIndices()->at(0) !=
			mp_context->getDevice()->getPresentationQueueIndices()->at(0)){
		swapChainInfo+=("MFVkSwapchainRes::initSwapChain - "
				"Using multiple queues! - Not testet!\n");
		mp_swapChainCreateInfo->imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		mp_swapChainCreateInfo->queueFamilyIndexCount = mp_context->getDevice()->getUsedQueueIndices()->size();
		std::vector<uint32_t> data(mp_context->getDevice()->getUsedQueueIndices()->begin(),mp_context->getDevice()->getUsedQueueIndices()->end());
		mp_swapChainCreateInfo->pQueueFamilyIndices = data.data();
	}else{
		swapChainInfo+=("MFVkSwapchainRes::initSwapChain - Using single queue with "
				"imageSharingMode=VK_SHARING_MODE_EXCLUSIVE\n");
		mp_swapChainCreateInfo->imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		mp_swapChainCreateInfo->pQueueFamilyIndices = nullptr;
		mp_swapChainCreateInfo->queueFamilyIndexCount = 0;
	}

	mp_swapChainCreateInfo->minImageCount = m_swapChainMinImageCount;
	mp_swapChainCreateInfo->imageFormat = mp_surfaceFormat->format;
	mp_swapChainCreateInfo->imageColorSpace = mp_surfaceFormat->colorSpace;
    mp_swapChainCreateInfo->imageExtent = m_swapChainExtent;
	mp_swapChainCreateInfo->imageArrayLayers = 1;
	mp_swapChainCreateInfo->imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	mp_swapChainCreateInfo->flags=0;
	mp_swapChainCreateInfo->pNext=nullptr;

	mp_swapChainCreateInfo->preTransform = mp_surfaceCapabilities->currentTransform;
	mp_swapChainCreateInfo->compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	mp_swapChainCreateInfo->presentMode = *mp_presentMode;
	mp_swapChainCreateInfo->clipped = VK_TRUE;
	mp_swapChainCreateInfo->surface = *(mp_context->getSurface());
	mp_swapChainCreateInfo->oldSwapchain=VK_NULL_HANDLE;

	VkResult result=vkCreateSwapchainKHR(
			*mp_context->getVirtualDevice(),
			mp_swapChainCreateInfo,
			nullptr,
			mp_swapChain);
    if (result != VK_SUCCESS) {
    	mp_context->printErr("MFVkSwapchainRes::initSwapChain() - "
    			"failed to create swap chain! VkResult="+std::to_string(result) +"\n"+swapChainInfo);
		return false;
	}


    result = vkGetSwapchainImagesKHR(
    		*mp_context->getVirtualDevice(),
    		*mp_swapChain,
			&m_swapChainImageCount,
			nullptr);
    if(result!=VK_SUCCESS){
    	mp_context->printErr(swapChainInfo+"MFVkSwapchainRes::initSwapChain() - "
    			"failed to access image count!");
    	return false;
    }
    mp_context->getSetup()->m_swapChainImageCount=m_swapChainImageCount;
    mp_vecSwapChainImage->resize(m_swapChainImageCount);
    mp_vecSwapChainImageView->resize(m_swapChainImageCount);
    result=vkGetSwapchainImagesKHR(
    		*mp_context->getVirtualDevice(),
			*mp_swapChain,
			&m_swapChainImageCount,
			mp_vecSwapChainImage->data());
    swapChainInfo+=("MFVkSwapchainRes::initSwapChain - swapchain image count: "+
    		std::to_string(m_swapChainImageCount)+"\n");
    m_swapChainImageFormat = mp_surfaceFormat->format;
    swapChainInfo+=("MFVkSwapchainRes::initSwapChain - swapchain (format/colorspace): "+
    			std::to_string(m_swapChainImageFormat)+"/"+
				std::to_string(mp_surfaceFormat->colorSpace)+"\n");


    if((result != VK_SUCCESS && result != VK_INCOMPLETE ) || m_swapChainImageCount==0){
    	mp_context->printErr(swapChainInfo+"Failed to create swap chain image!");
    	return false;
    }
    if(result == VK_INCOMPLETE){
    	mp_context->printWarning(swapChainInfo+"Incomplete creation of swap chain images!");
    	return false;
    }
	m_fullInit=false;

	initImageViews();
	mp_context->printInfo(swapChainInfo);
	return ret;
}

bool MFVkSwapchainRes::initImageViews(){
	mp_context->printInfo("MFVkSwapchainRes::initImageViews");
	mp_vecWrappedImage->clear();
	for(uint32_t i = 0; i<mp_vecSwapChainImage->size();i++){
		MFVkImage *img=new MFVkImage(mp_context,mp_resources,mp_vecSwapChainImage->at(i));
		img->setWidth(m_swapChainExtent.width);
		img->setHeight(m_swapChainExtent.height);
		MFVkImage::setupSwapChainImage(mp_context,mp_resources,img);
		img->setImageView(&mp_vecSwapChainImageView->at(i));
		if(!img->createImageView()){
			mp_context->printErr("MFVkSwapchainRes::initImageViews - "
					"failed to create swap chain image view!");
			return false;
		}
		mp_vecWrappedImage->push_back(img);
	}
	return true;
}

bool MFVkSwapchainRes::initResources(){
	bool ret=true;
	uint32_t formatCount=0;
	uint32_t presentationModeCount=0;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
			*mp_context->getPhysicalDevice(),
			*mp_context->getSurface(),
			mp_surfaceCapabilities);
	m_swapChainExtent=mp_surfaceCapabilities->currentExtent;
	mp_context->printInfo("MFVkSwapchainRes::initResources() - "
			"sureface (swapchain) extent: "+
			std::to_string(m_swapChainExtent.width)+"/"+
			std::to_string(m_swapChainExtent.height));

	vkGetPhysicalDeviceSurfaceFormatsKHR(*mp_context->getPhysicalDevice(),*mp_context->getSurface(),&formatCount,nullptr);

	if(formatCount>0){
		mp_surfaceFormats->resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(*mp_context->getPhysicalDevice(),*mp_context->getSurface(),&formatCount,mp_surfaceFormats->data());
		std::string info="";
		for(VkSurfaceFormatKHR format:(*mp_surfaceFormats)){
			info+="\nFormat:"+std::to_string(format.format);
			info+=" ColorSpace:";
			info+=std::to_string(format.colorSpace);
		}
		mp_context->printInfo("MFVkSwapchainRes::initResources() - Found "+
				std::to_string(mp_surfaceFormats->size())+" surface formats:"+info);
	}else{
		mp_context->printErr("MFVkSwapchainRes::initResources - "
				"physical device got no surface formats!");
		ret=false;
		return false;
	}

	vkGetPhysicalDeviceSurfacePresentModesKHR(*mp_context->getPhysicalDevice(), *mp_context->getSurface(), &presentationModeCount, nullptr);
	if(presentationModeCount>0){
		mp_presentModes->resize(presentationModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(*mp_context->getPhysicalDevice(), *mp_context->getSurface(), &presentationModeCount, mp_presentModes->data());
		mp_context->printInfo("MFVkSwapchainRes::initResources - "
				"PresentMode count: "+std::to_string(presentationModeCount));
	}else{
		mp_context->printErr("MFVkSwapchainRes::initResources - "
				"physical device got no surface present modes!");
		return false;
	}
	return ret;
}

void MFVkSwapchainRes::initSurfaceFormat(){
	bool isSet=false;
	for(VkSurfaceFormatKHR format:*mp_surfaceFormats){
		if(format.format == mp_context->getSetup()->m_surfaceFormat &&
				format.colorSpace == mp_context->getSetup()->m_surfaceColorSpace){
			*mp_surfaceFormat=format;
			isSet=true;
			break;
		}
	}

	if(!isSet){
		mp_context->printWarning("MFVkSwapChain::initSurfaceFormat - "
				"no suitable surface format found! "
				"Check the MFVkSetup object setup within MFVkContext!");
		*mp_surfaceFormat=(*mp_surfaceFormats)[0];
	}
}

void MFVkSwapchainRes::initPresentMode(){
	*mp_presentMode = VK_PRESENT_MODE_FIFO_KHR;

	for(VkPresentModeKHR presentMode:*mp_presentModes){
		if(presentMode == mp_context->getSetup()->m_swapPresentMode){
			*mp_presentMode=mp_context->getSetup()->m_swapPresentMode;
			mp_context->printInfo("MFVkSwapchainRes::initPresentMode - present mode set to: "+
					std::to_string(presentMode));
			return;
		}
		if(presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR){
			*mp_presentMode=VK_PRESENT_MODE_IMMEDIATE_KHR;
		}
	}
	mp_context->printWarning("MFVkSwapchainRes::initPresentMode - "
			"couldn't set present mode to setup value! "
			"Current mode (VkPresentModeKHR): "+std::to_string(*mp_presentMode));
}

void MFVkSwapchainRes::initExtent(){
	mp_context->printInfo("MFVkSwapchainRes::initExtent() - sureface capabilities set by vulkan! width:"+std::to_string(m_swapChainExtent.width)+" height:"+std::to_string(m_swapChainExtent.height));
}

