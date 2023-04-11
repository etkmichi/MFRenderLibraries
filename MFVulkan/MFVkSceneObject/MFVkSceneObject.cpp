/*
 * MFVkInterfaceSceneObject.cpp
 *
 *  Created on: 27.08.2019
 *      Author: michl
 */

#include "MFVkSceneObject.h"

MFVkSceneObject::MFVkSceneObject() {
	mp_uboContainer=nullptr;
	mp_vertexShader=nullptr;
	mp_fragmentShader=nullptr;
	mp_pipeline=nullptr;
	mp_descriptor=nullptr;
	mp_vertexBuffer=nullptr;
	mp_indexBuffer=nullptr;
	mp_descriptorSetLayout=nullptr;
	mp_colorBuffer=new UBOVec4();
	mp_vertexDataCollection=nullptr;
	mp_geometry=nullptr;
	mp_sceneObjectBuffers=new MFVkSceneObjectBuffers();
}

MFVkSceneObject::~MFVkSceneObject() {
	// TODO Auto-generated destructor stub
}
void MFVkSceneObject::setColor(glm::vec4 color){
	mp_colorBuffer->data=color;
}
void MFVkSceneObject::setUBOBuffer(S_UniformBuffer* pUBOContainer){
		mp_uboContainer=pUBOContainer;
};
VkDescriptorSet* MFVkSceneObject::getDescriptorSet(){return mp_descriptor;};
S_UniformBuffer* MFVkSceneObject::getUBOBuffer(){return mp_uboContainer;};

MFVkSceneObjectBuffers* MFVkSceneObject::getBufferCollection(){
	return mp_sceneObjectBuffers;
};

MFVkShader* MFVkSceneObject::getVertexShader(){return mp_vertexShader;};
MFVkShader* MFVkSceneObject::getFragmentShader(){return mp_fragmentShader;};
MFVkPipeline* MFVkSceneObject::getPipeline(){return mp_pipeline;};

void MFVkSceneObject::setVertexShader(MFVkShader* shader){mp_vertexShader=shader;};
void MFVkSceneObject::setFragmentShader(MFVkShader* shader){
	mp_vertexShader=shader;};
void MFVkSceneObject::setDescriptorSet(VkDescriptorSet* pDescriptor){mp_descriptor=pDescriptor;};
void MFVkSceneObject::setPipeline(MFVkPipeline* pipeline){mp_pipeline=pipeline;};

void MFVkSceneObject::setVertexDataCollection(MFVkVertexDataCollection* pVertexDataCollection){
	mp_vertexDataCollection=pVertexDataCollection;
}
MFVkVertexDataCollection* MFVkSceneObject::getVertexDataCollection(){return mp_vertexDataCollection;};

void MFVkSceneObject::setVertexBuffer(
		MFVkBuffer* vertexBuffer,
		uint32_t verticesCount){
	m_verticesCount=verticesCount;
	mp_vertexBuffer=vertexBuffer;
}
void MFVkSceneObject::setIndexBuffer(
		MFVkBuffer* indexBuffer,
		uint32_t indicesCount){
	m_indicesCount=indicesCount;
	mp_indexBuffer=indexBuffer;
}
MFVkBuffer* MFVkSceneObject::getVertexBuffer(){
	return mp_vertexBuffer;
}
MFVkBuffer* MFVkSceneObject::getIndexBuffer(){
	return mp_indexBuffer;
}


void MFVkSceneObject::updateVulkanBuffer(){
	for(void* pBuf:*mp_uboContainer->pVecBuffers){
		MFVkBuffer* pVkBuf=(MFVkBuffer*)pBuf;
		pVkBuf->updateBuffer();
	}
}

//void MFVkInterfaceSceneObject::setVertexDataCollection(
//		MFVkVertexDataCollection* pVertexDataCollection){
//	mp_vertexDataCollection=pVertexDataCollection;
//}
