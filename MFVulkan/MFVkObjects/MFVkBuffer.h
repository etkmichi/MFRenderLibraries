/*
 * MFVkBuffer.h
 *
 *  Created on: 19.08.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKOBJECTS_MFVKBUFFER_H_
#define MFVULKAN_MFVKOBJECTS_MFVKBUFFER_H_

class MFVkBuffer;

#include "vulkan/vulkan.h"
#include "../MFVkContext/MFVkContext.h"
#include "../MFVkResources/MFVkResources.h"

/**
 * This class provides vulkan buffer functionalities. Buffers can be created and more with more eas.
 */
class MFVkBuffer {
private:
	VkMappedMemoryRange memRange;
	MFVkContext
		*mp_context;
	MFVkResources
		*mp_resources;

	VkDeviceSize
		m_bufferSize;
	VkBuffer
		*mp_buffer;
	VkDeviceMemory
		*mp_bufferMemory;
	MFVkBuffer
		*mp_stagingBuffer=nullptr;

	VkDescriptorSet
		*mp_descSet=nullptr;

	VkBufferUsageFlags
		m_usage;
	VkMemoryPropertyFlags
		m_memoryProperties;

	VkBufferCreateInfo
		m_bufferInfo;

	VkMemoryRequirements
		*mp_memRequirements;
	void
		*mp_inputData;

	bool
		m_isBufferCreated=false,
		m_isDataMapped=false,
		m_isExternalBuffer = false,
		m_isBound=false,
		m_isValid=true;
	uint32_t
		m_memOffset=0;

public:
	std::string bufferName="";
	MFVkBuffer(MFVkContext *pContext,
			MFVkResources *pResources,
			uint64_t bufferSize);

	/**
	 * This constructor allows to wrap around an existing buffer and its memory. The destructor will not
	 * destroy the vulkan resources they must be destroyed manually or with a call to destroyBuffer().
	 * @param pContext
	 * @param pResources
	 * @param vkBuffer - pointer to a buffer handle
	 * @param vkBufferMemory pointer to a buffer memory handle
	 * @param bufferSize - size which will be used during creation
	 */
	MFVkBuffer(MFVkContext *pContext,
			MFVkResources *pResources,
			VkBuffer* vkBuffer,
			VkDeviceMemory* vkBufferMemory,
			uint64_t bufferSize);
	virtual ~MFVkBuffer();

	/**
	 * This function will destroy the buffer and free its memory if it was successfully created before (with createBuffer()).
	 */
	void destroyBuffer();

	/**
	 * Creates the vulkan buffer with its memory and the setup of this object.
	 * @return true if successful
	 */
	bool createBuffer();

	/**
	 * After a buffer was created, the data can be mapped and copied with this function.
	 * @param in_data
	 */
	bool mapData(void* in_data);

	/**
	 * Invalidation will prevent the usage of the buffer by this classes functions.
	 * Invalidation can not be undone, a buffer must be recreated for valid
	 * function access!
	 */
	void invalidate(){m_isValid=false;};

	/**
	 * If data was mapped at least one time with a valid in_data
	 * (mapData(void* in_data), this function will use the same in data ptr to
	 * update the vulkan buffer.
	 */
	bool updateBuffer();

	/**
	 * Returns a pointer to the input data which was coppied to the vulkan buffer.
	 * Returns the pointer to the last mapped data of mapData(void* in_data) if used.
	 * If no data was coppied, nullptr will be returned.
	 */
	void* getInData(){return mp_inputData;};
	void setInData(void* inData){mp_inputData=inData;};
	bool bindMemory();
	bool isBound(){return m_isBound;};

	/**
	 * Coppies the given buffer to the inner buffer of this object. Size will be matched
	 * to the given buffer. If the inner buffer was not created, this function will create it!
	 * @param pIn_buffer input buffer wich may contains data. This buffer should be created and filled with data (createBuffer(), mapData(...))
	 */
	bool copyBuffer(MFVkBuffer *pIn_buffer);
	void setUsage(VkBufferUsageFlags usage);
	void setMemoryProperties(VkMemoryPropertyFlags properties);
	void setSize(VkDeviceSize size);
	VkDeviceSize getSize(){return m_bufferSize;};
	uint32_t getMemoryOffset(){return m_memOffset;};
	void setMemoryOffset(uint32_t offset){m_memOffset=offset;};
	VkBuffer* getBuffer(){return mp_buffer;};

	VkDeviceMemory* getBufferMemory(){return mp_bufferMemory;};

	VkDescriptorSet* getDescriptorSet(){return mp_descSet;};

	/**
	 * Sets the descriptor set. The descriptor set connects the buffer to a
	 * uniform value.
	 * @param pDescSet
	 */
	void setDesriptorSet(VkDescriptorSet* pDescSet){mp_descSet=pDescSet;};

	bool isCreated(){return m_isBufferCreated;};

	bool setupAsDeviceBuffer();
	bool setupAsStagingBuffer();
	MFVkBuffer* getStagingBuffer(){return mp_stagingBuffer;};


	static MFVkBuffer* createStagingBuffer(MFVkContext* pContext,MFVkResources* pRes,VkDeviceSize size);
};

#endif /* MFVULKAN_MFVKOBJECTS_MFVKBUFFER_H_ */
