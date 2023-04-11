/*
 * MFUBOManager.cpp
 *
 *  Created on: 28.08.2019
 *      Author: michl
 */

#include "MFVkUBOManager.h"

MFVkUBOManager::MFVkUBOManager(MFRenderContext* pContext) {
  mp_context=pContext;
  if(mp_context==nullptr){
    mp_context=MFRenderContext::smp_staticContext;
  }
	mp_vecUBOSetup = new std::vector<MFDataObject>();
	mp_vecInitialDataDummies=new std::vector<void*>();
	mp_vecUniformBuffers = new std::vector<S_UniformBuffer*>();
	mp_vecUBOMemory=new std::vector<void*>();
	mp_vecVkBuffers=new std::vector<MFVkBuffer*>();
}

MFVkUBOManager::~MFVkUBOManager() {
	//TODO delete S_Uni... and sub resources
	for(void* data:*mp_vecInitialDataDummies){
		free(data);
	}
	delete mp_vecInitialDataDummies;
	for(S_UniformBuffer* pB:*mp_vecUniformBuffers){
		for(void* pSubBuffer:*(pB->pVecBuffers)){
			MFVkBuffer* pVkBuf=(MFVkBuffer*)pSubBuffer;
			pVkBuf->destroyBuffer();
			pVkBuf->invalidate();
			delete pVkBuf;
	//			delete pVkBuf; TODO delete buffer if it is save to be deleted
		}
		pB->pVecBuffers->clear();
		delete pB->pVecBuffers;
		delete pB;
		pB=nullptr;
	}
	mp_vecUniformBuffers->clear();
	delete mp_vecUniformBuffers;
	clearAllocatedUBOBuffer();
}

void MFVkUBOManager::destroySubBuffers(){
	for(S_UniformBuffer* pB:*mp_vecUniformBuffers){
		for(void* pSubBuffer:*(pB->pVecBuffers)){
			MFVkBuffer* pVkBuf=(MFVkBuffer*)pSubBuffer;
			pVkBuf->destroyBuffer();
//			delete pVkBuf; TODO delete buffer if it is save to be deleted
		}
	}
}

void MFVkUBOManager::clearAllocatedUBOBuffer(){
	for(void* mem:*mp_vecUBOMemory){
		free(mem);
	}
	mp_vecUBOMemory->clear();
}

bool MFVkUBOManager::allocateHostUniformMemory(){
	if(mp_vecUBOMemory->size()<1){//allocate from scratch
		for(MFDataObject uboData:*mp_vecUBOSetup){
			uint32_t dataSize=uboData.getDataSize();
			uint32_t memSize=m_maxObjectCount*dataSize;
			if(uboData.isStatic()){
				memSize=uboData.getDataSize();
			}
			void* uboPtr=malloc(memSize);
			if(uboPtr==nullptr){
				mp_context->printErr("MFVkUBOManager::initUboManager - "
						"Failed to allocate mem for ubo data!");
				return false;
			}
			mp_vecUBOMemory->push_back(uboPtr);
			if(uboData.isStatic()){
				//static memory will use one buffer for all objects.
				memcpy(uboPtr,uboData.getInitialData(),dataSize);
				continue;
			}
			for(uint32_t i=0;i<m_maxObjectCount;i++){//copy the initial value to mem.
				uint32_t dataOffset=dataSize*i;
				uint8_t* uboDataPtr=(uint8_t*)uboPtr;
				uboDataPtr+=dataOffset;
				memcpy(uboDataPtr,uboData.getInitialData(),dataSize);
			}
		}
	}
	return true;
}

bool MFVkUBOManager::initUboManager(MFVkContext* pContext, MFVkResources* pResources){
	if(mp_vecUniformBuffers->size()>0){
		destroySubBuffers();
	}
	m_nextBufferIndex=0;

	/*Allocate memory at one piece for each buffer type*/
	if(!allocateHostUniformMemory()){
		return false;
	}

	for(uint32_t i=0;i<m_maxObjectCount;i++){
		S_UniformBuffer *uniformData=nullptr;
		if(i<mp_vecUniformBuffers->size()){
			uniformData=mp_vecUniformBuffers->at(i);
		}else{
			uniformData=new S_UniformBuffer();
			mp_vecUniformBuffers->push_back(uniformData);
		}
		uniformData->init();
		uniformData->pVecUBOSetup=mp_vecUBOSetup;
		uint32_t indexUBOMem=0;
		for(MFDataObject uboData:*mp_vecUBOSetup){
			uint32_t dataSize=uboData.getDataSize();
			uint32_t dataOffset=dataSize*i;
			MFVkBuffer* pMFBuffer=nullptr;
			//if a setup is static, buffers will share the input data.
			if(uboData.isStatic()){
				//if buffer data shall be static, use one buffer for
				//all objects
				dataOffset=0;
				if(i==0){//create first buffer at index 0
					pMFBuffer=new MFVkBuffer(
							pContext,
							pResources,
							dataSize);
					mp_vecVkBuffers->push_back(pMFBuffer);
					pMFBuffer->setUsage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
					pMFBuffer->setMemoryProperties(
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
						VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
					pMFBuffer->createBuffer();
					uint8_t* uboDataPtr=(uint8_t*)mp_vecUBOMemory->at(indexUBOMem);
					pMFBuffer->mapData(uboDataPtr);
				}else{
					//use existing buffer at index 0
					pMFBuffer=(MFVkBuffer*)
						mp_vecUniformBuffers->at(0)->pVecBuffers->at(indexUBOMem);
				}
				uniformData->addBuffer(pMFBuffer);
				indexUBOMem++;
				continue;
			}

			//buffer is not static
			if(indexUBOMem >= uniformData->pVecBuffers->size()){
				//no buffer for data type created -> create new one
				pMFBuffer=new MFVkBuffer(
					pContext,
					pResources,
					dataSize);//TODO allocate mem at one piece?
				mp_vecVkBuffers->push_back(pMFBuffer);
				uniformData->addBuffer(pMFBuffer);
			}else{/*Buffer should be available*/
				pMFBuffer=(MFVkBuffer*)uniformData->pVecBuffers->at(indexUBOMem);
				if(pMFBuffer==nullptr){
					pMFBuffer=new MFVkBuffer(
						pContext,
						pResources,
						dataSize);//TODO allocate mem at one piece?
					mp_vecVkBuffers->push_back(pMFBuffer);
				}
			}
			pMFBuffer->setUsage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
			pMFBuffer->setMemoryProperties(
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
					VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			pMFBuffer->createBuffer();
			uint8_t* uboDataPtr=(uint8_t*)mp_vecUBOMemory->at(indexUBOMem);
			uboDataPtr+=dataOffset;
			pMFBuffer->mapData(uboDataPtr);
			indexUBOMem++;
		}
	}
	return true;
}

bool MFVkUBOManager::recreateBuffers(){
	bool ret=true;
	for(MFVkBuffer* pBuffer:*mp_vecVkBuffers){
		if(!pBuffer->createBuffer()){
			mp_context->printErr(" MFVkUBOManager::recreateBuffers() - "
					"failed to recreate a buffer!!");
			ret=false;
		}else{
			pBuffer->updateBuffer();
		}
	}
	return ret;
}
S_UniformBuffer* MFVkUBOManager::getUBOBuffer(uint32_t index){
	return mp_vecUniformBuffers->at(index);
}
S_UniformBuffer* MFVkUBOManager::getNextUBOBuffer(){
	if(m_nextBufferIndex>=m_maxObjectCount){
		mp_context->printErr("MFVkUBOManager::getNextUBOContainer - "
				"max object count reached!!!!!");
		return nullptr;
	}
	S_UniformBuffer* bufferContainer=mp_vecUniformBuffers->at(m_nextBufferIndex);
	bufferContainer->index=m_nextBufferIndex;
	m_nextBufferIndex++;
	if(m_nextBufferIndex>=m_maxObjectCount){
		mp_context->printWarning("MFVkUBOManager::getNextUBOContainer - "
				"max object count reached! Next return will be nullptr!");
	}
	return bufferContainer;
}

void MFVkUBOManager::setMaxUBOCount(uint32_t count){
	m_maxObjectCount=count;
}

void MFVkUBOManager::addDataType(MFDataObject* pDataObject){
	if(pDataObject==nullptr){
		mp_context->printErr("MFVkUBOManager::addDataType - "
				"pDataObject==nullptr");
		return;
	}
	if(pDataObject->getInitialData()==nullptr){
		void* data=malloc(pDataObject->getDataSize());
		pDataObject->setInitialData(data);
		mp_vecInitialDataDummies->push_back(data);
	}
	mp_vecUBOSetup->push_back(*pDataObject);
}

void MFVkUBOManager::addDataType4x4(glm::mat4x4* initialData){
	MFDataObject nextData;
	nextData.setDataByteSize(sizeof(glm::mat4x4));
	if(initialData==nullptr){
		void* data=malloc(nextData.getDataSize());
		glm::mat4* dataMat=static_cast<glm::mat4*>(data);
		*dataMat=glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,0.0f,0.0f));
		nextData.setInitialData(data);
		mp_vecInitialDataDummies->push_back(data);
	}else{
		nextData.setInitialData(initialData);
	}
	mp_vecUBOSetup->push_back(nextData);
}

void MFVkUBOManager::addDataTypeUINT32(uint32_t* initialData){
	MFDataObject nextData;
	nextData.setDataByteSize(sizeof(uint32_t));
	if(initialData==nullptr){
		void* data=malloc(nextData.getDataSize());
		nextData.setInitialData(data);
		mp_vecInitialDataDummies->push_back(data);
	}else{
		nextData.setInitialData(initialData);
	}
	mp_vecUBOSetup->push_back(nextData);
}
