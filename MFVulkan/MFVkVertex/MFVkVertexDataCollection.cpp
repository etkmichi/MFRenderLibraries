/*
 * MFVkVertexDataCollection.cpp
 *
 *  Created on: 04.12.2019
 *      Author: michl
 */

#include "MFVkVertexDataCollection.h"
#include <MFObject.h>
#include <MFBaseGeometries/MFGeometryData.h>
MFVkVertexDataCollection::MFVkVertexDataCollection(
		MFVkContext *pContext,
		MFVkResources *pResources){
	mp_context=pContext;
	mp_resources=pResources;
	mp_indexDataBuffer=nullptr;
	mp_vertexDataBuffer=nullptr;
	mp_vertexData=nullptr;
	mp_bindingDescription=new VkVertexInputBindingDescription();
	m_vertexDataSize=0;
	m_indexDataSize=0;
}
MFVkVertexDataCollection::MFVkVertexDataCollection(
		MFVertexDataCollection &other,
		MFVkContext *pContext,
		MFVkResources *pResources):
			MFVertexDataCollection(other){
	mp_context=pContext;
	mp_resources=pResources;
	mp_indexDataBuffer=nullptr;
	mp_vertexDataBuffer=nullptr;
	mp_vertexData=nullptr;
	mp_bindingDescription=new VkVertexInputBindingDescription();
	m_vertexDataSize=0;
	m_indexDataSize=0;
}

MFVkVertexDataCollection::~MFVkVertexDataCollection() {
	// TODO Auto-generated destructor stub
}

VkVertexInputBindingDescription* MFVkVertexDataCollection::createBindingDescription(){
	//TODO setup for more bindings
	if(mp_bindingDescription==nullptr){
		mp_bindingDescription=new VkVertexInputBindingDescription();
	}
	mp_bindingDescription->binding=0;
	mp_bindingDescription->inputRate=VK_VERTEX_INPUT_RATE_VERTEX;
	mp_bindingDescription->stride=mp_vecComplexVertices->at(0)->
			calculateStructureSize();
	return mp_bindingDescription;
}

std::vector<VkVertexInputAttributeDescription>*
	MFVkVertexDataCollection::createAttributeDescription(){
	if(mp_vecComplexVertices->size()<1){
		mp_context->printErr("MFVkVertexDataCollection::createAttributeDescription() - "
				"no adaptable vertex available for description setup!");
		return nullptr;
	}
	MFVkComplexVertex* pVertexInfo=(MFVkComplexVertex*)mp_vecComplexVertices->at(0);
	if(pVertexInfo->getVecData()->size()<1){
		mp_context->printErr("MFVkVertexDataCollection::createAttributeDescription() - "
				"No valid adaptable vertex at index 0! Setup adaptable vertex before"
				"creating the attribute description!");
		return nullptr;
	}
	std::vector<VkVertexInputAttributeDescription>* vecAttribDesc=new
			std::vector<VkVertexInputAttributeDescription>();
	uint32_t counter=0;
	uint32_t offsetCounter=0;
	for(MFDataObject* pData:*pVertexInfo->getVecData()){
		VkVertexInputAttributeDescription attribDesc={};
		attribDesc.binding=0;//TODO setup for more bindings
		attribDesc.location=counter;
		attribDesc.format=pVertexInfo->getVulkanFormat(E_DataType(pData->getDataType()));
		attribDesc.offset=offsetCounter;
		vecAttribDesc->push_back(attribDesc);
		offsetCounter+=pData->getDataSize();
		counter++;
	}
	return vecAttribDesc;
}

MFAdaptableVertex* MFVkVertexDataCollection::createNewVertex(){
	return new MFVkComplexVertex(mp_context);
}

MFVkBuffer* MFVkVertexDataCollection::createVertexBuffer(){
	setupComplexVertices();
	if(mp_vecComplexVertices->size()<1){
		mp_context->printErr("MFVkVertexDataCollection::createVulkanBuffer() - "
				"mp_vecComplexVertices->size()<1!");
		return nullptr;
	}
	m_vertexDataSize=mp_vecComplexVertices->at(0)->calculateStructureSize();
	uint32_t bufferSize=m_vertexDataSize*m_vertexCount;
	try{
		if(mp_vertexData==nullptr || m_allocatedMemSize!=bufferSize){
			if(mp_vertexData!=nullptr){
				free(mp_vertexData);
			}
			mp_vertexData=malloc(bufferSize);//checkbuffer size
			m_allocatedMemSize=bufferSize;
		}
	}catch(std::exception &e){
		m_vertexDataSize=0;
		mp_context->printErr("MFVkVertexDataCollection::createVulkanBuffer() - std"
				"failed (allocating or freeing memory)!");
		return nullptr;
	}
	/*Copy data to buffer*/
	uint32_t memOffset=0;
	uint32_t vertSize=0;
//	std::string debugInfo="";
	/*copy vertices to buffer*/
	for(MFAdaptableVertex* pVertex:*mp_vecComplexVertices){
		if(vertSize==0)vertSize=pVertex->calculateStructureSize();
//		debugInfo+=pVertex->toString();
		if(!pVertex->copyDataFromStructure(mp_vertexData,memOffset)){
			mp_context->printErr("MFVkVertexDataCollection::createVulkanBuffer() -"
					"sth failed while copying vetex data!");
		}
		memOffset+=vertSize;
	}
//	mp_context->printInfo(debugInfo);
	if(mp_vertexDataBuffer!=nullptr){
		mp_vertexDataBuffer->destroyBuffer();
		delete mp_vertexDataBuffer;
	}
	mp_vertexDataBuffer=createDeviceBuffer(//TODO check if a new one must be created
		bufferSize,
		mp_vertexData,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	return mp_vertexDataBuffer;
}

MFVkBuffer* MFVkVertexDataCollection::createIndexBuffer(){
	if(mp_vecIndices!=nullptr && mp_vecIndices->size()>0){
		m_indexDataSize=mp_vecIndices->size()*sizeof(uint32_t);
		mp_indexDataBuffer=createDeviceBuffer(
			m_indexDataSize,
			mp_vecIndices->data(),
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
	}
	return mp_indexDataBuffer;
}

MFVkBuffer* MFVkVertexDataCollection::createDeviceBuffer(
		VkDeviceSize bufferSize,
		void* data,
		VkBufferUsageFlags usage){
	MFVkBuffer* dataBuffer=new MFVkBuffer(
			mp_context,
			mp_resources,
			bufferSize);
	dataBuffer->setUsage(usage);
	if(!dataBuffer->setupAsDeviceBuffer()){
		mp_context->printErr("MFVkVertexDataCollection::createDeviceBuffer - "
				"failed to create data buffer!");
		delete dataBuffer;
		return nullptr;
	}
	if(!dataBuffer->mapData(data)){
		mp_context->printErr("MFVkObjectManager::createDeviceBuffer - "
				"failed to copy staging buffer to data buffer!");
		delete dataBuffer;
		return nullptr;
	}
	return dataBuffer;
}
