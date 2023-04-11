/*
 * MFVkShaderManager.h
 *
 *  Created on: 23.09.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKSCENEMANAGER_MFVKSHADERMANAGER_H_
#define MFVULKAN_MFVKSCENEMANAGER_MFVKSHADERMANAGER_H_

class MFVkShaderManager;

#include "../MFVkContext/MFVkContext.h"
#include "../MFVkResources/MFVkResources.h"
#include "../MFVkPredefinedShaders/MFVkDefaultShader.h"
#include "../MFPredefinedObjectSetups/MFAbstractSetup.h"
class MFVkShaderManager {
private:
	MFVkContext
		*mp_context;
	MFVkResources
		*mp_resources;

	MFAbstractSetup
		*mp_defaultSetup;

public:
	MFVkShaderManager(MFVkContext *pContext,MFVkResources* pResources);

	/**
	 * Initializes the managers resources and predefined setups.
	 * @return
	 */
	bool initShaderManager();
	virtual ~MFVkShaderManager();
};

#endif /* MFVULKAN_MFVKSCENEMANAGER_MFVKSHADERMANAGER_H_ */
