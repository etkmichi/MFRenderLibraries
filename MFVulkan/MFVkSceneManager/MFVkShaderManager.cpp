/*
 * MFVkShaderManager.cpp
 *
 *  Created on: 23.09.2019
 *      Author: michl
 */

#include "MFVkShaderManager.h"
#include <MFObjects/MFObject.h>

MFVkShaderManager::MFVkShaderManager(MFVkContext *pContext,MFVkResources* pResources) {
	mp_context=pContext;
	mp_resources=pResources;
	mp_defaultSetup=mp_resources->getDefaultObjectSetup();
}
bool MFVkShaderManager::initShaderManager(){
//	if(!mp_defaultSetup->initShader()){
//		mp_context->printErr("MFVkShaderManager::initShaderManager() - "
//				"failed to init default shader!");
//		return false;
//	}
	return true;
}

MFVkShaderManager::~MFVkShaderManager() {
	// TODO Auto-generated destructor stub
}

