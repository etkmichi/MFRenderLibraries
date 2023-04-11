/*
 * MFVkPipelineManager.cpp
 *
 *  Created on: 17.09.2019
 *      Author: michl
 */

#include "MFVkPipelineManager.h"

MFVkPipelineManager::MFVkPipelineManager(MFVkContext* pContext, MFVkResources *pResources) {
	mp_context=pContext;
	mp_resources = pResources;

	mp_vecPipelineContext = new std::vector<PipelineContext*>();
	mp_vecPipelines=new std::vector<MFVkPipeline*>();
	mp_defaultSetup=mp_resources->getDefaultObjectSetup();
}

MFVkPipelineManager::~MFVkPipelineManager() {
	// TODO Auto-generated destructor stub
}

void MFVkPipelineManager::destroyPipelines(){
	for(PipelineContext* pPipeContext:*mp_vecPipelineContext){
		MFVkPipeline* pPipe = (MFVkPipeline*)pPipeContext->pipeline;
		pPipe->destroyVulkanResources();
	}
}

bool MFVkPipelineManager::initPipelineManager(){
//	if(!mp_defaultSetup->initPipeline()){
//		mp_context->printErr("MFVkPipelineManager::initPipelineManager() - "
//				"Init default pipeline failed!");
//		return false;
//	}
	return true;
}

void MFVkPipelineManager::addPipeline(MFVkPipeline* pipeline,std::string description,bool destroyInManager){
	PipelineContext *pipeCon=new PipelineContext();
	pipeCon->destroyInManager=destroyInManager;
	pipeCon->pipeDescription=description;
	pipeCon->pipeline=(void*)(pipeline);
	m_lockVecPipeline.lock();
	pipeCon->index=mp_vecPipelineContext->size();
	mp_vecPipelineContext->push_back(pipeCon);
	m_lockVecPipeline.unlock();
}

PipelineContext* MFVkPipelineManager::addPipeline(std::string description,bool destroyInManager,PipelineSetup* pipeSetup){
	MFVkPipeline *pipeline=new MFVkPipeline(mp_context,pipeSetup);
	/*Use a single color implementation*/
	PipelineContext *pipeCon=new PipelineContext();
	pipeCon->destroyInManager=destroyInManager;
	pipeCon->pipeDescription=description;
	pipeCon->pipeline=(void*)(pipeline);
	m_lockVecPipeline.lock();
	pipeCon->index=mp_vecPipelineContext->size();
	mp_vecPipelineContext->push_back(pipeCon);
	m_lockVecPipeline.unlock();
	return pipeCon;
}

MFVkPipeline* MFVkPipelineManager::getPipeline(uint32_t index){
	m_lockVecPipeline.lock();
	MFVkPipeline *pipeline=(MFVkPipeline*)(mp_vecPipelineContext->at(index)->pipeline);
	m_lockVecPipeline.unlock();
	return pipeline;
}

PipelineContext* MFVkPipelineManager::getPipelineContext(uint32_t index){
	m_lockVecPipeline.lock();
	PipelineContext *pipeline=mp_vecPipelineContext->at(index);
	m_lockVecPipeline.unlock();
	return pipeline;
}

