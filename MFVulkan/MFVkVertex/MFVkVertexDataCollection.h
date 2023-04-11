/*
 * MFVkVertexDataCollection.h
 *
 *  Created on: 04.12.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKVERTEX_MFVKVERTEXDATACOLLECTION_H_
#define MFVULKAN_MFVKVERTEX_MFVKVERTEXDATACOLLECTION_H_
class MFVkVertexDataCollection;
#include <vulkan/vulkan.h>
#include <MFVertexClasses/MFVertexDataCollection.h>
#include "../MFVkObjects/MFVkBuffer.h"

/**
 * This class provides functionality to set up specific vertex data.
 */
class MFVkVertexDataCollection : public MFVertexDataCollection{
private:
	MFVkContext
		*mp_context;
	MFVkResources
		*mp_resources;

	MFVkBuffer
		*mp_vertexDataBuffer,
		*mp_indexDataBuffer;
	uint32_t
		m_vertexDataSize,
		m_indexDataSize;

	VkVertexInputBindingDescription
		*mp_bindingDescription;
	void
		*mp_vertexData;
	uint32_t
		m_allocatedMemSize=0;

    MFVkBuffer* createDeviceBuffer(VkDeviceSize bufferSize,void* data,VkBufferUsageFlags usage);

public:
    MFVkVertexDataCollection(MFVkContext *pContext,MFVkResources *pResources);
    MFVkVertexDataCollection(MFVertexDataCollection &other,MFVkContext *pContext,MFVkResources *pResources);
	virtual ~MFVkVertexDataCollection();

	/*Inherited function*/
	MFAdaptableVertex* createNewVertex();
	MFVkBuffer* createVertexBuffer();
	MFVkBuffer* createIndexBuffer();

	//TODO implement create shader
	//	- Automatically create advanced shader which can use the given data format
	//	of a collection
	//TODO static implementation of a lot of advanced setups
	//	- is that ok?

	uint32_t getVertexDataSize(){return m_vertexDataSize;};
	uint32_t getIndexDataSize(){return m_indexDataSize;};

	/**
	 *
	 * @return
	 */
	VkVertexInputBindingDescription* createBindingDescription();

	/**
	 * The attribute description is needed by the vulkan pipeline (MFVkPipeline and
	 * MFVkPipelineManager). This function iterates through the set up data of the
	 * underlying parent class (MFAdaptableVertex) object and creates the attrib. desc..
	 *
	 * @return
	 */
	std::vector<VkVertexInputAttributeDescription>* createAttributeDescription();

};

#endif /* MFVULKAN_MFVKVERTEX_MFVKVERTEXDATACOLLECTION_H_ */
