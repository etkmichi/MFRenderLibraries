/*
 * MFVkPipelineManager.h
 *
 *  Created on: 17.09.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKSCENEMANAGER_MFVKPIPELINEMANAGER_H_
#define MFVULKAN_MFVKSCENEMANAGER_MFVKPIPELINEMANAGER_H_

class MFVkPipelineManager;

#include "../MFVkObjects/MFVkStructs.h"
#include "../MFVkContext/MFVkContext.h"
#include "../MFVkResources/MFVkResources.h"
#include "../MFVkObjectComponents/MFVkPipeline.h"
#include "../MFPredefinedObjectSetups/MFAbstractSetup.h"

#include <vector>
#include <string>

/**
 * The pipline manager provides access to a setup of pre-defined pipelines. Depending on the type of object to render one of this
 * pipelines can be chosen.
 */
class MFVkPipelineManager {
private:
	MFVkContext
		*mp_context;
	MFVkResources
		*mp_resources;

	MFAbstractSetup
		*mp_defaultSetup;

	std::vector<PipelineContext*>
		*mp_vecPipelineContext;
	std::vector<MFVkPipeline*>
		*mp_vecPipelines;
	std::mutex
		m_lockVecPipeline;

public:
	/**
	 * @param pContext
	 * @param pResources
	 */
	MFVkPipelineManager(MFVkContext* pContext, MFVkResources *pResources);
	virtual ~MFVkPipelineManager();

	/**
	 * Iterates over the pipeline objects and destroys their resources
	 */
	void destroyPipelines();

	/**
	 * Initializes the managers resources and predefined setups.
	 * The descriptor manager within the given MFVkResources of the constructor must
	 * be initialized
	 * @return
	 */
	bool initPipelineManager();

	/**
	 * Creates a pipeline, adds it to the inner field and returns a pointer to a structure which contains the pipeline.
	 * @param description - description of the pipeline (fe usage)
	 * @param destroyInManager - true if the pipeline manager shall destroy / delete the pipeline within the destructor
	 * @return a pointer to a structure which contains the pipeline
	 */
	PipelineContext* addPipeline(std::string description,bool destroyInManager,PipelineSetup* pipeSetup);

	void addPipeline(MFVkPipeline* pipeline,std::string description,bool destroyInManager);
	MFVkPipeline* getPipeline(uint32_t index);

	/**
	 * The pipeline is stored wrapped with some context which may be useful! returns the context
	 * containing the pipeline and some information.
	 * @param index into the inner array
	 * @return nullptr if wrong index or a valid pointer to the structure containing the pipeline.
	 */
	PipelineContext* getPipelineContext(uint32_t index);

};

#endif /* MFVULKAN_MFVKSCENEMANAGER_MFVKPIPELINEMANAGER_H_ */
