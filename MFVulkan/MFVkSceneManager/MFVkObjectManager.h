/*
 * MFVkObjectManager.h
 *
 *  Created on: 27.08.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKSCENEMANAGER_MFVKOBJECTMANAGER_H_
#define MFVULKAN_MFVKSCENEMANAGER_MFVKOBJECTMANAGER_H_

class MFVkObjectManager;
#include <mutex>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>

#include "vulkan/vulkan.h"
#include "../MFVkObjects/MFVkStructs.h"
#include "../MFVkContext/MFVkContext.h"
#include "../MFVkResources/MFVkResources.h"
#include "../MFVkSceneObject/MFVkSceneObject.h"

#include "MFVkUBOManager.h"
#include "MFVkShaderManager.h"
#include "MFVkDescriptorManager.h"
#include "MFVkPipelineManager.h"

#include "../MFPredefinedObjectSetups/MFAbstractSetup.h"
#include "../MFVkVertex/MFVkVertexDataCollection.h"

class MFVkObjectManager {
	MFVkContext
		*mp_context;
	MFVkResources
		*mp_resources;

	MFAbstractSetup
		*mp_defaultSetup;

    MFVkDescriptorManager
		*mp_descriptorManager;

    MFVkShaderManager
		*mp_shaderManager;

    MFVkPipelineManager
		*mp_pipelineManager;

    std::mutex
		lockUBOManager;

    uint32_t
		m_maxObjectCount=1000;

    S_UniformBuffer
		*mp_viewBufferContainer,
		*mp_projectionBufferContainer;

    MFVkBuffer
		*mp_projectionMatrixBuffer,
		*mp_viewMatrixBuffer;

	glm::mat4
		m_view,
		m_proj;
public:
	MFVkObjectManager(MFVkContext *pContext,MFVkResources *pResources);//TODO add view & projection matrix buffers
	virtual ~MFVkObjectManager();
	void setViewMatrixBuffer(MFVkBuffer *projectionMatrixBuffer){mp_projectionMatrixBuffer=projectionMatrixBuffer;};
	void setProjectionMatrixBuffer(MFVkBuffer *viewMatrixBuffer){mp_viewMatrixBuffer=viewMatrixBuffer;};
	MFVkBuffer* getViewMatrixBuffer(){return mp_projectionMatrixBuffer;};
	MFVkBuffer* getProjectionMatrixBuffer(){return mp_viewMatrixBuffer;};
	void destroyBuffer();
	void destroyDescriptorPool();

	/**
	 * Initializes the ubo manager and the descriptor manager. Make sure to setup the object manager!
	 * @return true if successfull
	 */
	bool initObjectManager();
//	bool reinitUniformBuffers();
	void setMaxObjectCount(uint32_t count);
	bool addModel();
	bool addDefaultObject(
		MFVkSceneObject* pObject);
	bool addObject(
		MFVkSceneObject* pObject,
		MFAbstractSetup* pObjectSetup);
	/**
	 * This function adds a default object. It uses the MFVkDefaultSetup to set up the
	 * object.
	 * @param vertices
	 * @param indices
	 * @param color
	 * @return
	 */
	MFVkSceneObject* addDefaultObject(
		MFAbstractGeometry* pGeometry,
		uint32_t color);
	MFVkSceneObject* addDefaultObject(
		MFAbstractGeometry* pGeometry,
		glm::vec4 color);

	//TODO another thread which iterates over the added setups and adds the new one if
	//not existent.

	/*Used during initialization*/
	bool addDescriptor();

	/*TODO make thread safe*/
	/*MFVkDefaultSetup::createObject
	std::vector<MFVkBuffer*>* getVecColors();*/

	//TODO add function with MFAbstractSetup,MFAbstractGeometry,MFVkSceneObjectBuffers
//
//	bool setupDefaultObject(MFVkInterfaceSceneObject* object,UBOVec4 color);
//	bool updateDefaultDescriptorSet(VkDescriptorSet* descSet,
//			VkDescriptorBufferInfo *pBufferInfoModelMatrix,
//			VkDescriptorBufferInfo *pBufferInfoViewMatrix,
//			VkDescriptorBufferInfo *pBufferInfoProjectionMatrix,
//			VkDescriptorBufferInfo *pBufferInfoColorVec4);
//	MFVkInterfaceSceneObject* addDefaultObject

	/**
	 * If setup of the ubo manager needs to be changed, this method can be used to get the object.
	 * @return the ubo manager.
	 */

	MFVkDescriptorManager* getDescriptorManager(){return mp_descriptorManager;};
	MFVkPipelineManager* getPipelineManager(){return mp_pipelineManager;};
	//TODO adds a new descriptor layout to the descriptor manager and returns the setup class -> use this in MFVkResource initDesc..._3
	//MFVkDescriptorSetLayout addDescriptorSet();
	//TODO predefine descriptor layout add methods
	//addDSetImage...
};

#endif /* MFVULKAN_MFVKSCENEMANAGER_MFVKOBJECTMANAGER_H_ */
