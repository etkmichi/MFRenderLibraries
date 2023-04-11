/*
 * MFVkComplexVertex.cpp
 *
 *  Created on: 04.12.2019
 *      Author: michl
 */

#include "MFVkComplexVertex.h"
#include <MFObjects/MFObject.h>

MFVkComplexVertex::MFVkComplexVertex(MFRenderContext* pContext) {
  mp_context=pContext;
  if(mp_context==nullptr){
    mp_context=MFRenderContext::smp_staticContext;
  }
	init();
}
MFVkComplexVertex::MFVkComplexVertex(
    MFAdaptableVertex *pCopy,
    MFRenderContext* pContext) {
  mp_context=pContext;
  if(mp_context==nullptr){
    mp_context=MFRenderContext::smp_staticContext;
  }
	init();
	for(MFDataObject* pData:*pCopy->getVecData()){
		addData(*pData);
	}
}
void MFVkComplexVertex::init(){
	mp_mapVulkanFormats=new std::map<E_DataType,VkFormat>();
	(*mp_mapVulkanFormats)[DATA_TYPE_DUMMY]=VK_FORMAT_R8_UINT;
	(*mp_mapVulkanFormats)[DATA_TYPE_UINT8]=VK_FORMAT_R8_UINT;//VK_FORMAT_R8_UINT
	(*mp_mapVulkanFormats)[DATA_TYPE_UINT16]=VK_FORMAT_R16_UINT;
	(*mp_mapVulkanFormats)[DATA_TYPE_UINT32]=VK_FORMAT_R32_UINT;
	(*mp_mapVulkanFormats)[DATA_TYPE_UINT64]=VK_FORMAT_R64_UINT;
	(*mp_mapVulkanFormats)[DATA_TYPE_INT8]=VK_FORMAT_R8_SINT;
	(*mp_mapVulkanFormats)[DATA_TYPE_INT16]=VK_FORMAT_R16_SINT;
	(*mp_mapVulkanFormats)[DATA_TYPE_INT32]=VK_FORMAT_R32_SINT;
	(*mp_mapVulkanFormats)[DATA_TYPE_INT64]=VK_FORMAT_R64_SINT;
	(*mp_mapVulkanFormats)[DATA_TYPE_FLOAT32]=VK_FORMAT_R32_SFLOAT;
	(*mp_mapVulkanFormats)[DATA_TYPE_DOUBLE64]=VK_FORMAT_R64_SFLOAT;
	(*mp_mapVulkanFormats)[DATA_TYPE_VEC_2D]=VK_FORMAT_R32G32_SFLOAT;
	(*mp_mapVulkanFormats)[DATA_TYPE_VEC_3D]= VK_FORMAT_R32G32B32_SFLOAT;
	(*mp_mapVulkanFormats)[DATA_TYPE_VEC_4D]=	VK_FORMAT_R32G32B32A32_SFLOAT;
	(*mp_mapVulkanFormats)[DATA_TYPE_MAT_3X3]=VK_FORMAT_R32G32B32_SFLOAT;
	(*mp_mapVulkanFormats)[DATA_TYPE_MAT_4X4]=	VK_FORMAT_R32G32B32A32_SFLOAT;
	(*mp_mapVulkanFormats)[DATA_TYPE_N_BIT]=VK_FORMAT_B8G8R8A8_SINT;
	mp_bindingDescription=new VkVertexInputBindingDescription();
	m_vertexSize=0;
}

MFVkComplexVertex::~MFVkComplexVertex() {
}

VkVertexInputBindingDescription* MFVkComplexVertex::createBindingDescription(){
	//TODO setup for more bindings
	mp_bindingDescription->binding=0;
	mp_bindingDescription->inputRate=VK_VERTEX_INPUT_RATE_VERTEX;
	mp_bindingDescription->stride=calculateStructureSize();
	return mp_bindingDescription;
}

std::vector<VkVertexInputAttributeDescription>* MFVkComplexVertex::
		createAttributeDescription(){
  if(mp_context==nullptr){
    mp_context=MFRenderContext::smp_staticContext;
  }
	if(getVecData()->size()<1){
		mp_context->printErr("MFVkVertexDataCollection::createAttributeDescription() - "
				"No data type added! Setup adaptable vertex before "
				"creating the attribute description!");
		return nullptr;
	}
	std::vector<VkVertexInputAttributeDescription>* vecAttribDesc=new
			std::vector<VkVertexInputAttributeDescription>();
	uint32_t counter=0;
	uint32_t offsetCounter=0;
	for(MFDataObject* pData:*getVecData()){
		VkVertexInputAttributeDescription attribDesc={};
		attribDesc.binding=0;//TODO setup for more bindings
		attribDesc.location=counter;
		attribDesc.format=getVulkanFormat(E_DataType(pData->getDataType()));
		attribDesc.offset=offsetCounter;
		vecAttribDesc->push_back(attribDesc);
		offsetCounter+=pData->getDataSize();
		counter++;
	}
	return vecAttribDesc;
}

VkFormat MFVkComplexVertex::getVulkanFormat(E_DataType type){
	if(mp_mapVulkanFormats->count(type)>0){
		VkFormat ret=mp_mapVulkanFormats->at(type);
		return ret;
	}
	mp_context->printErr("MFVkComplexVertex::getVulkanFormat - "
			"no vulkan format specified! returning VK_FORMAT_R32G32B32A32_SFLOAT");
	return VK_FORMAT_R32G32B32A32_SFLOAT;
}
