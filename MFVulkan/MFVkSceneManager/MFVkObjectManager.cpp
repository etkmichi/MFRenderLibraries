/*
 * MFVkObjectManager.cpp
 *
 *  Created on: 27.08.2019
 *      Author: michl
 */

#include "MFVkObjectManager.h"
MFVkObjectManager::MFVkObjectManager(MFVkContext *pContext,MFVkResources *pResources) {
	mp_context=pContext;
	mp_resources=pResources;
	mp_viewMatrixBuffer=nullptr;
	mp_projectionBufferContainer=nullptr;
	mp_projectionMatrixBuffer=nullptr;
	mp_viewBufferContainer=nullptr;
	mp_descriptorManager=nullptr;
	mp_pipelineManager=nullptr;
	mp_shaderManager=nullptr;
	mp_defaultSetup=mp_resources->getDefaultObjectSetup();
}

MFVkObjectManager::~MFVkObjectManager() {
}

void MFVkObjectManager::destroyDescriptorPool(){
	mp_descriptorManager->destroyDescriptorPool();
}

void MFVkObjectManager::setMaxObjectCount(uint32_t count){
	m_maxObjectCount=count;
}

bool MFVkObjectManager::initObjectManager(){
//	mp_uboManager = mp_resources->getUBOManager();
	mp_shaderManager = mp_resources->getShaderManager();
	mp_descriptorManager = mp_resources->getDescriptorManager();
	mp_pipelineManager = mp_resources->getPipelineManager();
	if(mp_shaderManager==nullptr || mp_descriptorManager==nullptr ||
			mp_pipelineManager==nullptr){
		//mp_uboManager==nullptr ||
		mp_context->printInfo("MFVkResources must be initialized!");
		return false;
	}
	return true;
}

MFVkSceneObject* MFVkObjectManager::addDefaultObject(
		MFAbstractGeometry* pGeometry,
		uint32_t color){
	if(!pGeometry->isCreated()){
		if(!pGeometry->initGeometry()){
			mp_context->printErr("MFVkObjectManager::addDefaultObject - "
					"failed to create geometry!");
			return nullptr;
		}
	}
	MFVkSceneObject* newObject=new MFVkSceneObject();
	newObject->setGeometry(pGeometry);
	UBOVec4* pBufferColor=new UBOVec4();
	pBufferColor->data[0]	=((color&0xFF000000)>>24)/255.0f;
	pBufferColor->data[1]	=((color&0x00FF0000)>>16)/255.0f;
	pBufferColor->data[2]	=((color&0x0000FF00)>>8)/255.0f;
	pBufferColor->data[3]	=((color&0x000000FF)>>0)/255.0f;
	newObject->setColor(pBufferColor->data);

	//TODO setup by MFVkDefaultSetup
	if(!mp_defaultSetup->setupObject(newObject)){
		mp_context->printErr("MFVkObjectManager::addDefaultObject - "
				"failed to setup default object!");
		delete newObject;
		return nullptr;
	}
	return newObject;
}

MFVkSceneObject* MFVkObjectManager::addDefaultObject(
		MFAbstractGeometry* pGeometry,
		glm::vec4 color){
	if(!pGeometry->isCreated()){
		if(!pGeometry->initGeometry()){
			mp_context->printErr("MFVkObjectManager::addDefaultObject - "
					"failed to create geometry!");
			return nullptr;
		}
	}
	MFVkSceneObject* newObject=new MFVkSceneObject();
	newObject->setGeometry(pGeometry);
	UBOVec4* pBufferColor=new UBOVec4();
	pBufferColor->data=color;
	newObject->setColor(pBufferColor->data);

	//TODO setup by MFVkDefaultSetup
	if(!mp_defaultSetup->setupObject(newObject)){
		mp_context->printErr("MFVkObjectManager::addDefaultObject - "
				"failed to setup default object!");
		delete newObject;
		return nullptr;
	}
	return newObject;
}

bool MFVkObjectManager::addDefaultObject(MFVkSceneObject* pObject){
	if(mp_defaultSetup==nullptr)return false;
	if(!mp_defaultSetup->setupObject(pObject)){
		mp_context->printErr("MFVkObjectManager::addDefaultObject("
				"MFVkSceneObject* pObject) - "
				"failed to setup default object!");
		return false;
	}
	return true;
}

bool MFVkObjectManager::addObject(
		MFVkSceneObject* pObject,
		MFAbstractSetup* pObjectSetup){
	if(pObjectSetup==nullptr || pObject==nullptr){
		mp_context->printErr("MFVkObjectManager::addObject("
				"MFVkSceneObject* pObject,MFAbstractSetup* pObjectSetup) - "
				"pObjectSetup==nullptr || pObject==nullptr!");
		return false;
	}
	if(!mp_defaultSetup->setupObject(pObject)){
		mp_context->printErr("MFVkObjectManager::addDefaultObject("
				"MFVkSceneObject* pObject,MFAbst...) - "
				"failed to setup object!");
		return false;
	}
	return true;
}
