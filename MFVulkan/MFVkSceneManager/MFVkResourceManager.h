/*
 * MFVkResourceManager.h
 *
 *  Created on: 25.11.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKSCENEMANAGER_MFVKRESOURCEMANAGER_H_
#define MFVULKAN_MFVKSCENEMANAGER_MFVKRESOURCEMANAGER_H_
class MFVkResourceManager;
#include "../MFVkContext/MFVkContext.h"
#include "../MFVkResources/MFVkResources.h"
#include <vector>
//TODO implement managers with this base class
class MFVkResourceManager {
private:
	std::vector<MFVkResourceManager*>
		*mp_vecPriorManagers;
protected:
	virtual bool initResources(){return false;};
	MFVkContext* mp_context;
	MFVkResources* mp_resources;
public:
	MFVkResourceManager(MFVkContext* pContext,MFVkResources* pResources);
	virtual ~MFVkResourceManager();
	virtual bool recreateResources(){return false;};
	bool initManager();
	void addPriorInit(MFVkResourceManager* pPriorManager);

};

#endif /* MFVULKAN_MFVKSCENEMANAGER_MFVKRESOURCEMANAGER_H_ */
