/*
 * MFAbstractSetup.cpp
 *
 *  Created on: 12.12.2019
 *      Author: michl
 */

#include "MFAbstractSetup.h"

//MFAbstractSetup::MFAbstractSetup(){
//	mp_context=MFRenderContext::smp_staticContext;
//	mp_resources = nullptr;
//
////	mp_vertexDataOptimizer=new MFVertexDataOptimizer();
////	mp_vertexSetup=new MFVkComplexVertex();
//	mp_vertexDataCollection=new MFVkVertexDataCollection(mp_context,mp_resources);
//
//	/*pipeline*/
//	mp_pipelineSetup=new PipelineSetup();
//	mp_pipeline=new MFVkPipeline(mp_context,mp_pipelineSetup);
//
//	/*shader*/
//	mp_vertexShader	= new MFVkShader(mp_context);
//	mp_fragmentShader=new MFVkShader(mp_context);
//
//	/*descriptor*/
//	isDescriptorWritesCreated=false;
//	mp_vecDescriptorBinding = new std::vector<VkDescriptorSetLayoutBinding>();
//	mp_descriptorSetLayout	= new VkDescriptorSetLayout();
//	mp_vecDescriptorSet		= new std::vector<VkDescriptorSet>();
//	mp_descriptorPool		= new VkDescriptorPool();
//	mp_descSetLayouts=new std::vector<VkDescriptorSetLayout>();
//
//	/*objects*/
//	mp_uboManager=new MFVkUBOManager();
//	mp_uboManager->setMaxUBOCount(m_objectCount);
//	mp_vecObjects = new std::vector<MFVkSceneObject*>();
//	mp_vecWriteMasks=new std::vector<VkWriteDescriptorSet>();
//
//	mp_vertexModel=new MFVkComplexVertex();
//
//	isVertexShaderCreated=false;
//	isFragmentShaderCreated=false;
//	isDescriptorSetLayoutCreated=false;
//	isDescriptorSetCreated=false;
//	isDescriptorLayoutBindingCreated=false;
//	isDescriptorPoolCreated=false;
//	isUBOManagerCreated=false;
//	isVertexModelCreated=false;
//	mp_renderPass=nullptr;
//}
MFAbstractSetup::MFAbstractSetup(MFVkContext* pContext, MFVkResources *pResources){
	mp_context=pContext;
	mp_resources = pResources;

//	mp_vertexDataOptimizer=new MFVertexDataOptimizer();
//	mp_vertexSetup=new MFVkComplexVertex();
	mp_vertexDataCollection=new MFVkVertexDataCollection(mp_context,mp_resources);

	/*pipeline*/
	mp_pipelineSetup=new PipelineSetup();
	mp_pipeline=new MFVkPipeline(pContext,mp_pipelineSetup);

	/*shader*/
	mp_vertexShader	= new MFVkShader(mp_context);
	mp_fragmentShader=new MFVkShader(mp_context);

	/*descriptor*/
	isDescriptorWritesCreated=false;
	mp_vecDescriptorBinding = new std::vector<VkDescriptorSetLayoutBinding>();
	mp_descriptorSetLayout	= new VkDescriptorSetLayout();
	mp_vecDescriptorSet		= new std::vector<VkDescriptorSet>();;
	mp_descriptorPool		= new VkDescriptorPool();
	mp_descSetLayouts=new std::vector<VkDescriptorSetLayout>();

	/*objects*/
	mp_uboManager=new MFVkUBOManager(mp_context);
	mp_uboManager->setMaxUBOCount(m_objectCount);
	mp_vecObjects = new std::vector<MFVkSceneObject*>();
	mp_vecWriteMasks=new std::vector<VkWriteDescriptorSet>();

	mp_vertexModel=new MFVkComplexVertex(mp_context);

	isVertexShaderCreated=false;
	isFragmentShaderCreated=false;
	isDescriptorSetLayoutCreated=false;
	isDescriptorSetCreated=false;
	isDescriptorLayoutBindingCreated=false;
	isDescriptorPoolCreated=false;
	isUBOManagerCreated=false;
	isVertexModelCreated=false;
	mp_renderPass=nullptr;
}

MFAbstractSetup::~MFAbstractSetup() {
	mp_pipeline->destroyVulkanResources();
	delete mp_pipeline;
	delete mp_descriptorSetLayout;//TODO delete other stuff
}
/*resource destruction*/
void MFAbstractSetup::destroyPipeline(){
	mp_pipeline->destroyVulkanResources();
}

void MFAbstractSetup::destroyUBOs(){
	mp_uboManager->destroySubBuffers();
}
void MFAbstractSetup::destroyDescriptorPool(){
	vkDestroyDescriptorPool(*mp_context->getVirtualDevice(),
			*mp_descriptorPool,
			nullptr);
}

/*Resources reinit*/
bool MFAbstractSetup::reinitPipeline(){
	createPipelineSetup(mp_pipelineSetup);
	if(!mp_pipeline->createPipelineLayout())return false;
	if(!mp_pipeline->createPipeline())return false;
	return true;
}

bool MFAbstractSetup::reinitUBO(){
	if(!isUBOManagerCreated){
		mp_context->printErr("MFAbstractSetup::reinitUBO() - "
				"to reinit the ubo manager, initUBOManager() must be called before!");
		return false;
	}
	return reinitUBOManager(mp_uboManager);
}
bool MFAbstractSetup::reinitUBOManager(MFVkUBOManager* pUboMgr){
	return pUboMgr->recreateBuffers();
}
bool MFAbstractSetup::reinitDescriptors(){
	isDescriptorPoolCreated=false;
	if(!createDescriptorPool())return false;
	if(!createDescriptorSet())return false;
	mp_pipelineSetup->DescriptorSetLayoutPointer=mp_descriptorSetLayout;
	return true;
}
bool MFAbstractSetup::reinitSceneObjects(){
	uint32_t counter=0;
	for(MFVkSceneObject* pObject:*mp_vecObjects){
		std::vector<void*>* pBuffers=pObject->getUBOBuffer()->pVecBuffers;
		for(void* pBuf:*pBuffers){
			MFVkBuffer* pVkBuf=(MFVkBuffer*)pBuf;
			pVkBuf->setDesriptorSet(&(mp_vecDescriptorSet->at(counter)));
		}
		counter++;
		if(!reinitObject(pObject)){
			mp_context->printErr("MFAbstractSetup::reinitSceneObjects() - "
					"failed to reinit object!");
		}
	}
	return true;
}
bool MFAbstractSetup::reinitObject(MFVkSceneObject* pObject){
	std::vector<void*>* pBuffers=pObject->getUBOBuffer()->pVecBuffers;
	bool ret=true;
	if(pBuffers!=nullptr){
		ret&=updateDescriptorSet(pBuffers);
	}else{
		mp_context->printErr("MFAbstractSetup::reinitObject - "
				"No buffer available for reinit!");
	}
	return ret;
};

/**/
bool MFAbstractSetup::initUBOManager(){
	setupUBOManager();
	return isUBOManagerCreated;
}
bool MFAbstractSetup::setupUBOManager(){
	if(isUBOManagerCreated){
		mp_context->printWarning("MFAbstractSetup::setupUBOManager - "
				"The UBO manager was already set up! Use reinitUBO() for recreation"
				"of destroyed buffers!");
		return false;
	}
	isUBOManagerCreated=setupUBOManager(mp_uboManager);
	if(!isUBOManagerCreated)return false;
	isUBOManagerCreated=mp_uboManager->initUboManager(mp_context, mp_resources);
	return isUBOManagerCreated;
}

void MFAbstractSetup::setRenderPass(VkRenderPass* pRenderPass){
	mp_renderPass=pRenderPass;
	mp_pipelineSetup->pRenderPass=pRenderPass;
}
/*object functions */
bool MFAbstractSetup::setupRenderCommands(VkCommandBuffer* pBuffer){
	bool ret=true;
  lockVecObjects.lock();
	for(MFVkSceneObject* pSObj:*mp_vecObjects){
		if(pSObj->isDisabled()){
			continue;
		}
		bool ret2=setupRenderCommand(pBuffer, pSObj);
		if(!ret2){
			mp_context->printErr("MFAbstractSetup::setupRenderCommands - failed"
					"to record an object's render commands! Object name: "+
					pSObj->getName());
			ret=false;
		}
	}
  lockVecObjects.unlock();
	return ret;
}

bool MFAbstractSetup::setupObject(MFVkSceneObject* object){//TODO test from sc thread
	/*buffer for model matrix*/
	//create ubo manager for colors! ->
	//change class MFVkUBOManager to a class with template that UBOVec4 can be used
	if(mp_vertexShader== nullptr || mp_fragmentShader==nullptr){
		mp_context->printErr("MFAbstractSetup::setupObject - "
				"vertexShader&|fragShader==nullptr");
		return false;
	}
	VkDescriptorSet* pDescSet=getNextDescriptor();
	if(pDescSet==nullptr){
		mp_context->printErr("MFAbstractSetup::setupObject - pDescSet==nullptr");
		return false;
	}

	/*call virtual function for creating data collection:*/
	MFVkVertexDataCollection *pVertexData=createDataCollection(object);

	//TODO createDataCollection on separated thread and before starting the engine
	// - preloading data cause of high calculation time
	//	- Stick the data collection to the geometry -> if same geo its the same dc?
//			- no the dc must not be sticked to geo, but can be reused in some other way
	if(pVertexData==nullptr){
		mp_context->printErr("MFAbstractSetup::setupObject - pVertexData==nullptr");
		return false;
	}
	MFVkBuffer* pVertexBuffer=pVertexData->createVertexBuffer();
	MFVkBuffer* pIndexBuffer=pVertexData->createIndexBuffer();
	object->setVertexBuffer(
			pVertexBuffer,
			object->getGeometry()->getVecVertices().size());
	object->setIndexBuffer(
			pIndexBuffer,
			object->getGeometry()->getVecIndices().size());
	object->setVertexShader(mp_vertexShader);
	object->setFragmentShader(mp_fragmentShader);
	object->setDescriptorSetLayout(mp_descriptorSetLayout);
	object->setPipeline(mp_pipeline);
	object->setDescriptorSet(pDescSet);//TODO set all descriptor related stuff...
	S_UniformBuffer* pUB=mp_uboManager->getNextUBOBuffer();
	object->setUBOBuffer(pUB);
	//TODO setup so's MFVkSceneObjectBuffers to match the buffers of S_UniformBuffer*
	std::vector<void*>* pBuffers=pUB->pVecBuffers;
	//pUB->pVecBuffers;
	//test for using same buffer for multiple objects:
	//	std::vector<void*>* pBuffers=mp_vecObjects->at(0)->getUBOBuffer()->pVecBuffers;
	for(void* pBuf:*pBuffers){
		MFVkBuffer* pVkBuf=(MFVkBuffer*)pBuf;
		pVkBuf->setDesriptorSet(pDescSet);
	}
	updateDescriptorSet(pBuffers);
  lockVecObjects.lock();
  mp_vecObjects->push_back(object);
  object->setName("DefObj"+std::to_string(mp_vecObjects->size()));
  lockVecObjects.unlock();

  /*virtual function for setting up the ubo buffers provided by pUB*/
	if(!setupSOUniformCollection(
			object->getBufferCollection(),
			pUB,
			object)){
		mp_context->printErr("MFAbstractSetup::setupObject - "
				"Failed to setup scene object buffers! Buffers of the"
				"MFVkSceneObjectBuffers are not valid!");
		object->getBufferCollection()->initDummySetup();
	}
	return true;
}

bool MFAbstractSetup::updateDescriptorSet(std::vector<void*> *pBuffers){
	lockVecWriteMasks.lock();
	if(pBuffers->size()!=mp_vecWriteMasks->size()){
		mp_context->printErr("MFAbstractSetup::updateDescriptorSet - "
				"pBuffers->size()!=mp_vecWriteMasks->size() ("+
				std::to_string(pBuffers->size())+"!="+
				std::to_string(mp_vecWriteMasks->size())+
				") updating descriptor"
				"failed!!");
		lockVecWriteMasks.unlock();
		return false;
	}

	std::vector<VkDescriptorBufferInfo*> *pBufferInfos=
					new std::vector<VkDescriptorBufferInfo*>();
	uint32_t counter=0;
	for(void* pBuffer:*pBuffers){
		MFVkBuffer* pVkBuf=(MFVkBuffer*)pBuffer;
		VkDescriptorBufferInfo* pBufInfo=new VkDescriptorBufferInfo();
		pBufInfo->offset=pVkBuf->getMemoryOffset();
		pBufInfo->range = pVkBuf->getSize();
		pBufInfo->buffer = *pVkBuf->getBuffer();
		pBufferInfos->push_back(pBufInfo);
		mp_vecWriteMasks->at(counter).pBufferInfo=pBufInfo;
		mp_vecWriteMasks->at(counter).dstSet=*(pVkBuf->getDescriptorSet());
		if(mp_vecWriteMasks->at(counter).dstSet==nullptr){
			mp_context->printErr(" MFAbstractSetup::updateDescriptorSet - "
					"Failed! MFVkBuffer::getDescriptorSet==nullptr!!");
		}
		counter++;
	}

	vkUpdateDescriptorSets(
			*mp_context->getVirtualDevice(),
			static_cast<uint32_t>(mp_vecWriteMasks->size()),
			mp_vecWriteMasks->data(),
			0,
			nullptr);
	lockVecWriteMasks.unlock();

	for(VkDescriptorBufferInfo* pBufInfo:*pBufferInfos){
		delete pBufInfo;//TODO test if error occures
	}
	delete pBufferInfos;
	return true;
}

/*shader functions*/
bool MFAbstractSetup::createShader(){
	createVertexShader();
	createFragmentShader();
	return isVertexShaderCreated && isFragmentShaderCreated;
}

bool MFAbstractSetup::createVertexShader(){
	isVertexShaderCreated=createVertexShader(mp_vertexShader);
	return isVertexShaderCreated;
}

bool MFAbstractSetup::createFragmentShader(){
	isFragmentShaderCreated=createFragmentShader(mp_fragmentShader);
	return isFragmentShaderCreated;
}

MFVkShader* MFAbstractSetup::getVertexShader(){
	return mp_vertexShader;
}

MFVkShader* MFAbstractSetup::getFragmentShader(){
	return mp_fragmentShader;
}

/*descriptor functions*/
bool MFAbstractSetup::createDescriptorWrites(){
	mp_vecWriteMasks->clear();
	isDescriptorWritesCreated=createDescriptorWrites(mp_vecWriteMasks);
	return isDescriptorWritesCreated;
}

bool MFAbstractSetup::createVertexModel(){
	mp_vertexModel->clear();
	isVertexModelCreated=createVertexModel(mp_vertexModel);
	return isVertexModelCreated;
}

bool MFAbstractSetup::initDescriptorSetup(){
	mp_context->printInfo("MFAbstractSetup::initDescriptorSetup() - "
			"Creating descriptor stuff");
	if(!createVertexModel()){
		mp_context->printErr("MFAbstractSetup::initDescriptorSetup() - "
				"createVertexModel() failed!");
		return false;
	}
	if(!createDescriptorLayoutBindings()){
		mp_context->printErr("MFAbstractSetup::initDescriptorSetup() - "
				"createDescriptorLayoutBindings() failed!");
		return false;
	}
	if(!createDescriptorSetLayout()){
		mp_context->printErr("MFAbstractSetup::initDescriptorSetup() - "
				"createDescriptorSetLayout() failed!");
		return false;
	}

	if(!createDescriptorPool()){
		mp_context->printErr("MFAbstractSetup::initDescriptorSetup() - "
				"createDescriptorPool() failed!");
		return false;
	}

	if(!createDescriptorSet()){
		mp_context->printErr("MFAbstractSetup::initDescriptorSetup() - "
				"createDescriptorSet() failed!");
		return false;
	}
	if(!createDescriptorWrites()){
		mp_context->printErr("MFAbstractSetup::initDescriptorSetup() - "
				"createDescriptorWrites() failed!");
		return false;
	}
	m_nextDescriptor=0;
	return true;
}

bool MFAbstractSetup::createDescriptorSetLayout(){
	isDescriptorSetLayoutCreated=createDescriptorSetLayout(mp_descriptorSetLayout);
	if(isDescriptorSetLayoutCreated){
		mp_descSetLayouts->resize(m_objectCount);
		for(uint32_t i=0;i<m_objectCount;i++){
			mp_descSetLayouts->data()[i]=(*mp_descriptorSetLayout);
		}
	}
	return isDescriptorSetLayoutCreated;
};

bool MFAbstractSetup::createDescriptorSet(){
	if(mp_descriptorSetLayout==nullptr){
		mp_context->printErr("MFVkDescriptorManager::createDescriptorSet() - "
				"mp_descriptorSetLayout==nullptr!");
		return false;
	}
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = *mp_descriptorPool;/*the pool was defined with the count of the specified descriptor sets. For each descriptor used at least one is allocated in the pool*/
	allocInfo.descriptorSetCount = m_objectCount;
	allocInfo.pSetLayouts = mp_descSetLayouts->data();/*for every descriptor set one layout! here all layouts are the same.*/
//	if(mp_vecDescriptorSet!=nullptr){
//		mp_context->printInfo("MFVkDescriptorManager::createDefaultSet() - "
//				"deleting old data");//TODO vk function for freeing resources
//		delete mp_vecDescriptorSet;
//	}
	mp_vecDescriptorSet->resize(mp_descSetLayouts->size());
	mp_context->printInfo("MFVkDescriptorManager::createDefaultSet() - "
			"Creating default descriptor sets for objects");
	VkResult result = vkAllocateDescriptorSets(
			*mp_context->getVirtualDevice(),
			&allocInfo,
			mp_vecDescriptorSet->data());
	if (result != VK_SUCCESS) {
		mp_context->printErr("MFVkDescriptorManager::createDefaultSet() - "
				"failed to allocate descriptor sets for objects!"+std::to_string(result));
		return false;
	}
	return true;
}

bool MFAbstractSetup::createDescriptorPool(){
	isDescriptorPoolCreated=createDescriptorPool(mp_descriptorPool);
	return isDescriptorPoolCreated;
}

bool MFAbstractSetup::createDescriptorLayoutBindings(){
	mp_vecDescriptorBinding->clear();//TODO clear ok?=
	isDescriptorLayoutBindingCreated=createDescriptorLayoutBindings(
			mp_vecDescriptorBinding);
	return isDescriptorLayoutBindingCreated;
}

VkDescriptorSet* MFAbstractSetup::getNextDescriptor(){
	if(mp_vecDescriptorSet->size()<=m_nextDescriptor){
		mp_context->printErr("MFVkDescriptorManager::getNextDefaultDescriptor() - No descriptor set available!");
		return nullptr;
	}
	VkDescriptorSet* pDesc=&(mp_vecDescriptorSet->at(m_nextDescriptor));
	m_nextDescriptor++;
	return pDesc;
}

/*pipeline*/
bool MFAbstractSetup::initPipeline(){
	createPipelineSetup(mp_pipelineSetup);
	if(!mp_pipeline->initPipelineResources()){
		mp_context->printErr("MFVkDefaultSetup::initPipeline - "
				"mp_pipeline->initPipelineResources() failed!");
		return false;
	}

	if(!mp_pipeline->initPipeline()){
		mp_context->printErr("MFVkDefaultSetup::initPipeline - "
				"mp_pipeline->initPipeline() failed!");
		return false;
	}
	return true;
}

MFVkContext* MFAbstractSetup::getContext(){return mp_context;};
MFVkResources* MFAbstractSetup::getResources(){return mp_resources;};
MFVkUBOManager* MFAbstractSetup::getUBOManager(){return mp_uboManager;};
void MFAbstractSetup::setMaxObjectCount(uint32_t maxObjects){m_objectCount=maxObjects;};
uint32_t MFAbstractSetup::getMaxObjectCount(){return m_objectCount;};
VkDescriptorPool* MFAbstractSetup::getDescriptorPool(){return mp_descriptorPool;};
VkDescriptorSetLayout* MFAbstractSetup::getDescriptorLayout(){return mp_descriptorSetLayout;};
MFVkPipeline* MFAbstractSetup::getPipeline(){return mp_pipeline;};
PipelineSetup* MFAbstractSetup::getPipelineSetup(){return mp_pipelineSetup;};
