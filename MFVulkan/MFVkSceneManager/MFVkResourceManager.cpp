/*
 * MFVkResourceManager.cpp
 *
 *  Created on: 25.11.2019
 *      Author: michl
 */

#include "MFVkResourceManager.h"

MFVkResourceManager::MFVkResourceManager(MFVkContext* pContext,MFVkResources* pResources) {
	mp_context=pContext;
	mp_resources=pResources;
	mp_vecPriorManagers=new std::vector<MFVkResourceManager*>();
}

MFVkResourceManager::~MFVkResourceManager() {
	// TODO Auto-generated destructor stub
}

