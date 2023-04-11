/*
 * MFVkBuffer.cpp
 *
 *  Created on: 19.08.2019
 *      Author: michl
 */

#include "MFVkBuffer.h"

MFVkBuffer::MFVkBuffer(
		MFVkContext *pContext,
		MFVkResources *pResources,
		uint64_t bufferSize) {
	mp_context=pContext;
	mp_resources=pResources;
	mp_buffer = new VkBuffer();
	mp_bufferMemory = new VkDeviceMemory();
	m_bufferSize=bufferSize;
	m_bufferInfo={};
	m_memoryProperties={};
	m_usage={};
	m_bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	m_bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	m_bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	m_bufferInfo.flags = 0;
	mp_memRequirements=new VkMemoryRequirements();
	mp_inputData=nullptr;
}

MFVkBuffer::MFVkBuffer(MFVkContext *pContext,
		MFVkResources *pResources,
		VkBuffer* vkBuffer,
		VkDeviceMemory* vkBufferMemory,
		uint64_t bufferSize){
	mp_context=pContext;
	mp_resources=pResources;
	mp_buffer = vkBuffer;
	mp_bufferMemory = vkBufferMemory;
	m_bufferSize=bufferSize;
	m_bufferInfo={};
	m_memoryProperties={};
	m_usage={};
	m_bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	m_bufferInfo.size = bufferSize;
	m_bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	m_bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	m_bufferInfo.flags = 0;
	mp_memRequirements=new VkMemoryRequirements();
	m_isExternalBuffer=true;
	mp_inputData=nullptr;
}

MFVkBuffer::~MFVkBuffer() {
	if(!m_isExternalBuffer){
		if(m_isBufferCreated){
			destroyBuffer();
		}
		delete mp_bufferMemory;
		delete mp_buffer;
	}
	if(mp_stagingBuffer!=nullptr){
		mp_stagingBuffer->destroyBuffer();
		delete mp_stagingBuffer;
	}
	delete mp_memRequirements;
}

void MFVkBuffer::destroyBuffer(){
	if(m_isBufferCreated){
		vkDestroyBuffer(*mp_context->getVirtualDevice(), *mp_buffer, nullptr);
		vkFreeMemory(*mp_context->getVirtualDevice(), *mp_bufferMemory, nullptr);
		m_isBufferCreated=false;
	}
}

MFVkBuffer* MFVkBuffer::createStagingBuffer(MFVkContext* pContext,MFVkResources* pRes,VkDeviceSize size){
	MFVkBuffer* buffer=new MFVkBuffer(pContext,pRes,size);
	buffer->setUsage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	buffer->setMemoryProperties(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
					VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	buffer->setSize(size);
	buffer->createBuffer();
	return buffer;
}
bool MFVkBuffer::createBuffer(){
	if(!m_isValid){
		mp_context->printErr("MFVkBuffer - invalidated!!");
		return false;
	}
	m_bufferInfo.size = m_bufferSize;
	m_bufferInfo.flags = 0;
	m_bufferInfo.queueFamilyIndexCount = 0;
	m_bufferInfo.pQueueFamilyIndices = nullptr;
	m_bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(
			*mp_context->getVirtualDevice(),
			&m_bufferInfo, nullptr,
			mp_buffer) != VK_SUCCESS) {
		mp_context->printErr("MFVkBuffer - failed to create buffer!");
		return false;
	}

	vkGetBufferMemoryRequirements(
			*mp_context->getVirtualDevice(),
			*mp_buffer,
			mp_memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = mp_memRequirements->size;

	uint32_t memIndex;
	if(!mp_context->getDevice()->getMemoryIndex(
			memIndex,mp_memRequirements,m_memoryProperties)){
		mp_context->printErr("MFVkBuffer::createBuffer()"
				" - failed to find memory index for memory requirements/properties!");
		vkDestroyBuffer(*mp_context->getVirtualDevice(), *mp_buffer, nullptr);
		return false;
	}

	allocInfo.memoryTypeIndex = memIndex;

	if (vkAllocateMemory(
			*mp_context->getVirtualDevice(),
			&allocInfo,
			nullptr,
			mp_bufferMemory) != VK_SUCCESS) {
		mp_context->printErr("MFVkBuffer - failed to allocate buffer memory!");
		return false;
	}

	m_isBound=false;
	bindMemory();
	m_isBufferCreated=true;
	return true;
}
bool MFVkBuffer::bindMemory(){
	if(!m_isValid){
		mp_context->printErr("MFVkBuffer - invalidated!!");
		return false;
	}
	if(m_isBound)
		return true;
	VkResult res=vkBindBufferMemory(
			*mp_context->getVirtualDevice(),*mp_buffer,*mp_bufferMemory, 0);
	if(res != VK_SUCCESS){
		mp_context->printErr("MFVkBuffer::bindMemory() - No memory on host or device side! vkBindBufferMemory returned "+std::to_string(res));
		return false;
	}
	m_isBound=true;
	return true;
}

bool MFVkBuffer::mapData(void* pIn_data){
	if(!m_isValid){
		mp_context->printErr("MFVkBuffer - invalidated!!");
		return false;
	}
	void* mappedData;
	//TODO create functions for copiing from device buffer to device buffer
	//TODO create function for copying from device buffer to host buffer
	//TODO create abstract class for memory copying and implement the upper TODO's
	//TODO use the abstract class to copy data
	if(mp_stagingBuffer==nullptr){
		vkMapMemory(*mp_context->getVirtualDevice(),
				*mp_bufferMemory,
				0,
				m_bufferSize,
				0,
				&mappedData);
		memcpy(mappedData, pIn_data, (size_t) m_bufferSize);
		vkUnmapMemory(*mp_context->getVirtualDevice(), *mp_bufferMemory);
	}else{
		if(!mp_stagingBuffer->mapData(pIn_data)){
			mp_context->printErr("MFVkBuffer::mapData(void* pIn_data) - "
					"failed to copy data to staging buffer!");
			return false;
		}
		if(!this->copyBuffer(mp_stagingBuffer)){
			mp_context->printErr("MFVkBuffer::mapData(void* pIn_data) - "
					"failed to copy buffer from staging buffer to this buffer!");
			return false;
		}
	}
	mp_inputData=pIn_data;
	return true;
}

bool MFVkBuffer::updateBuffer(){
	if(mp_inputData==nullptr){
		mp_context->printErr("MFVkBuffer::updateBuffer - "
				"no data will be updated! mp_inputData==nullptr!");
		return false;
	}
	return mapData(mp_inputData);
}
bool MFVkBuffer::copyBuffer(MFVkBuffer *pIn_buffer){
	if(!m_isValid){
		mp_context->printErr("MFVkBuffer - invalidated!!");
		return false;
	}
	m_bufferSize=pIn_buffer->getSize();
	if(!isCreated()){
		if(!createBuffer()){
			mp_context->printErr("MFVkBuffer::copyBuffer - couldnt copy buffer!");
			return false;
		}
	}VkBufferCopy copyRegion = {};
	copyRegion.srcOffset=0;
	copyRegion.dstOffset=0;
	copyRegion.size=m_bufferSize;
	if(!pIn_buffer->isBound()){
		pIn_buffer->bindMemory();
	}
	if(!isBound()){
		bindMemory();
	}
	VkCommandBuffer* singleTimeCMD=
			mp_resources->getCommandManager()->beginSingleTimeCommands();
	vkCmdCopyBuffer(*singleTimeCMD,*pIn_buffer->getBuffer(),*mp_buffer,1,&copyRegion);
	mp_resources->getCommandManager()->endSingleTimeCommands(singleTimeCMD);
	return true;
}

void MFVkBuffer::setUsage(VkBufferUsageFlags usage){
	m_usage=usage;
	m_bufferInfo.usage=usage;
}
void MFVkBuffer::setMemoryProperties(VkMemoryPropertyFlags properties){
	m_memoryProperties=properties;
}
void MFVkBuffer::setSize(VkDeviceSize size){
	m_bufferSize=size;
}

bool MFVkBuffer::setupAsDeviceBuffer(){
	if(mp_stagingBuffer==nullptr){
		mp_stagingBuffer=new MFVkBuffer(mp_context, mp_resources, m_bufferSize);
		if(!mp_stagingBuffer->setupAsStagingBuffer()){
			mp_context->printErr("MFVkBuffer::setupAsDeviceBuffer() - "
					"failed to create data staging buffer!");
			return false;
		}
	}
	setUsage(m_usage|VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	setMemoryProperties(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	if(!createBuffer()){
		mp_context->printErr("MFVkBuffer::setupAsDeviceBuffer() - "
				"failed to create data buffer!");
		return false;
	}
	return true;
}
bool MFVkBuffer::setupAsStagingBuffer(){
	//TODO test if staging buffer can be host vis. only
	this->setUsage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	this->setMemoryProperties(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
					VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	if(!this->createBuffer()){
		mp_context->printErr("MFVkBuffer::setupAsStagingBuffer() - "
				"failed to create data buffer!");
		return false;
	}
	mp_stagingBuffer=nullptr;
	return true;
}

