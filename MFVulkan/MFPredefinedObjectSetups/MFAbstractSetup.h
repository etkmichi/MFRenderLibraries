/*
 * MFAbstractSetup.h
 *
 *  Created on: 12.12.2019
 *      Author: michl
 *      TODO's
 *      - Create static fallback setup
 *      	- shall be used if one important init step fails
 *      	- add an error marker
 */

#ifndef MFVULKAN_MFPREDEFINEDOBJECTSETUPS_MFABSTRACTSETUP_H_
#define MFVULKAN_MFPREDEFINEDOBJECTSETUPS_MFABSTRACTSETUP_H_

class MFAbstractSetup;
#include "../MFVkContext/MFVkContext.h"
#include "../MFVkResources/MFVkResources.h"
#include "../MFVkObjectComponents/MFVkPipeline.h"
#include "../MFVkPredefinedShaders/MFVkDefaultShader.h"
#include "../MFVkSceneManager/MFVkUBOManager.h"
#include "../MFVkVertex/MFVkComplexVertex.h"
#include "../MFVkVertex/MFVkVertexDataCollection.h"
#include "../MFVkSceneObject/MFVkSceneObject.h"
#include "../MFVkSceneObject/MFVkSceneObjectBuffers.h"
#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>


class MFAbstractSetup {
private:
  std::mutex
  lockVecObjects;
  std::vector<MFVkSceneObject*>
    *mp_vecObjects;
protected:
	MFVkContext
		*mp_context;
	MFVkResources
		*mp_resources;

	MFVkUBOManager
		*mp_uboManager;
	uint32_t
		m_objectCount=2000;

	/*shader data*/
	MFVkShader
		*mp_vertexShader,
		*mp_fragmentShader;

	/*pipeline data*/
	PipelineSetup
		*mp_pipelineSetup;
	MFVkPipeline
		*mp_pipeline;

	/*descriptor set data*/
	std::vector<VkDescriptorSetLayoutBinding>
		*mp_vecDescriptorBinding;
	VkDescriptorSetLayout
		*mp_descriptorSetLayout;
	std::vector<VkDescriptorSetLayout>
		*mp_descSetLayouts;
	std::vector<VkDescriptorSet>
		*mp_vecDescriptorSet;
	VkDescriptorPool
		*mp_descriptorPool;
	uint32_t
		m_nextDescriptor=0;
	std::vector<VkWriteDescriptorSet>
		*mp_vecWriteMasks;

	MFVkVertexDataCollection
		*mp_vertexDataCollection;
	MFVkComplexVertex
		*mp_vertexModel;

	VkRenderPass
		*mp_renderPass;

	glm::mat4
		m_view,
		m_proj;

	std::mutex
		lockVecWriteMasks;

	bool
		isDescriptorSetLayoutCreated,
		isDescriptorSetCreated,
		isDescriptorLayoutBindingCreated,
		isDescriptorPoolCreated,
		isDescriptorWritesCreated,
		isVertexShaderCreated,
		isFragmentShaderCreated,
		isVertexModelCreated,
		isUBOManagerCreated;
public:/*virtual functions of MFAbstractSetup*/
	/**
	 * Sets up pUBOManager to provdie implementation specific buffers.
	 * F.e. single colored objects which use model matrix, view matrix, perspective matrix and color:
      pUBOManager->addDataType(&modelMat);
      pUBOManager->addDataType(&viewMat);
      pUBOManager->addDataType(&perspectiveMat);
      pUBOManager->addDataType(&colorStructure);//Color
	 * @param pUBOManager - manager which shall provide specific data sturcture for his buffers.
	 * @return
	 */
	virtual bool setupUBOManager(MFVkUBOManager* pUBOManager){return false;};

	virtual bool initShader(){return false;};
public://TODO render pass with shared resources
//	MFAbstractSetup();
	MFAbstractSetup(MFVkContext* pContext, MFVkResources *pResources);
	virtual ~MFAbstractSetup();

	/*virtual functions*/

	/**
	 * This function uses the objects geometry to create the vertex data collection.
	 * The collection provides the vertex and index buffer for a scene object. This
	 * function is used during the setupObject(...) call, which will set the objects
	 * vertex and index buffer after creating the collection.
	 * @param object
	 * @return
	 */
	virtual MFVkVertexDataCollection* createDataCollection(MFVkSceneObject* pObject){return nullptr;};

	/**
	 * Creates a model which specifies all vertex related data.
	 * F.e. Vertex related data: Points ,Normals, Indices, Color, Color damping...
	 * @return
	 */
	virtual bool createVertexModel(MFVkComplexVertex* pModel){return false;};
	virtual bool createVertexShader(MFVkShader* pVertexShader){return false;};
	virtual bool createFragmentShader(MFVkShader* pFragmentShader){return false;};
	virtual bool createDescriptorSetLayout(VkDescriptorSetLayout *pDescriptorSetLayout){return false;};
	virtual bool createDescriptorPool(VkDescriptorPool *pPool){return false;};
	virtual bool createDescriptorLayoutBindings(std::vector<VkDescriptorSetLayoutBinding>* pVecBindings){return false;};
	virtual bool createDescriptorWrites(std::vector<VkWriteDescriptorSet>* pWrites){return false;};
	virtual void createPipelineSetup(PipelineSetup* setup){};
	virtual MFVkBuffer* getViewBuffer(){return nullptr;};

	/**
	 * Sets up the render command for one specific scene object. Uses the resources
	 * of this objects MFAbstractSetup implementation.
	 * @param pBuffer - buffer to record the drawing commands for the scene object
	 * @param pObject - the scene object, which provides additional resources for
	 * drawing (like uniform values, color, ...).
	 * @return true if successfull.
	 */
	virtual bool setupRenderCommand(VkCommandBuffer* pBuffer,MFVkSceneObject* pObject){return false;};

	/**
	 * This function will setup the MFVkSceneObjectBuffer (pSOBuffers) collection to point to
	 * the used buffers of ubo. S_UniformBuffer* ubo is a buffer provided by a specific implementation
	 * of MFAbstactSetup::setupUBOManager(). The scene objects
	 * used buffers depend on the implementation of MFAbstractSetup, the implementation
	 * should set the MFVkSceneObjectBuffers predefined pointer to the available
	 * buffers provided by (S_UniformBuffer*) ubo.
	 * @param pSOBuffers - buffers which will be set up by this function.
	 * The inner fields are pointer to implementation specific buffer structure
	 * (MFAbstactSetup::setupUBOManager).
	 * @param ubo - contains the buffer pointers to implementation specific data structure
	 * @return
	 */
	virtual bool setupSOUniformCollection(
			MFVkSceneObjectBuffers* pSOBuffers,
			S_UniformBuffer* ubo,
			MFVkSceneObject* pSceneObject){
	  MFObject::printWarning("MFAbstractSetup::setupSOUniformCollection - no impl.!");
	  return false;
	};

	/**
	 * Can be implemented for more specific reinitialization.
	 * Abstract implementation will call uboManager->recreateBuffers().
	 * @param uboManager - uboManager for recreation.
	 * @return true if recreation was successfull
	 */
	virtual bool reinitUBOManager(MFVkUBOManager* uboManager);

	/*resource destruction*/
	void destroyPipeline();
	void destroyUBOs();
	void destroyDescriptorPool();

	/*Resources reinit*/
	/**
	 * Reinit the pipeline differs from initPipeline! the reinit will only
	 * call some creation functions of the MFVkPipeline object while the
	 * initPipeline creates the pipeline from scratch.
	 * @return true if successfull
	 */
	bool reinitPipeline();
	bool reinitUBO();
	bool reinitDescriptors();
	bool reinitObject(MFVkSceneObject* pObject);
	bool reinitSceneObjects();

	MFVkContext* getContext();
	MFVkResources* getResources();

	/*render pass -> do not implement render pass setup int object setup*/
	void setRenderPass(VkRenderPass* pRenderPass);

//	bool setupRenderPass();
//	void destroyRenderPass();
//	bool createRenderPass();
//	virtual bool setupRenderPass(MFVkRenderPass* pRenderPass){return false;};
//	bool reinitRenderPass();

	/*UBO manager*/
	MFVkUBOManager* getUBOManager();
	bool setupUBOManager();

	/*init functions*/
	bool initRenderPass();
	/**
	 * Init function
	 * @return
	 */
	bool initPipeline();
	bool initUBOManager();
	bool initDescriptorSetup();

	/*Object related functions*/
	/**
	 * Calls the subclasse's implementation of setupRenderCommand with all already
	 * recorded MFVkSceneObject objects.
	 * @param pBuffer
	 * @return true if all objects were updated. false if one failed.
	 */
	bool setupRenderCommands(VkCommandBuffer* pBuffer);
	/**
	 * The max object count must be set before initialization! Default object count is
	 * 200. For every object buffers will be allocated during init, even if the objects
	 * are not defined yet!
	 * @param maxObjects
	 */
	void setMaxObjectCount(uint32_t maxObjects);
	uint32_t getMaxObjectCount();

	/**
	 * The setup function will prepare the scene object to be used. This function
	 * calls createDataCollection(object), getNextDescriptor(),
	 * createObjectUniformBuffer() and updateDescriptorSet(), all resources must
	 * be created before using this function!
	 * The scnene object will be change, changes to the scene object should be done
	 * after calling setupObject, other wise they may be overwritten.
	 * This function uses following setters for the scene object:
	 * object->setVertexBuffer
	 * object->setIndexBuffer
	 * object->setVertexShader(mp_vertexShader);
	 * object->setFragmentShader(mp_fragmentShader);
	 * object->setDescriptorSetLayout(mp_descriptorSetLayout);
	 * object->setName("Default setup object nr.std::to_string(mp_vecObjects->size()));
	 * object->setPipeline(mp_pipeline);
	 * object->setDescriptorSet(pDescSet);
	 * @param object
	 * @return
	 */
	bool setupObject(MFVkSceneObject* object);

	bool updateDescriptorSet(std::vector<void*> *pBuffers);

	bool createVertexModel();

	/*Shader related functions*/
	bool createShader();
	bool createVertexShader();

	bool createFragmentShader();

	MFVkShader* getVertexShader();
	MFVkShader* getFragmentShader();

	/*descriptor set functions*/
	bool createDescriptorSetLayout();
	bool createDescriptorSet();
	bool createDescriptorPool();
	bool createDescriptorLayoutBindings();
	bool createDescriptorWrites();

	VkDescriptorPool* getDescriptorPool();
	VkDescriptorSet* getNextDescriptor();
	VkDescriptorSetLayout* getDescriptorLayout();

	/*pipeline functions*/
	/**
	 * Make sure to call initPipeline before!
	 * @return the pointer to the  pipeline with the  setup.
	 */
	MFVkPipeline* getPipeline();
	PipelineSetup* getPipelineSetup();
	VkRenderPass* getRenderPass(){return mp_renderPass;};
};

#endif /* MFVULKAN_MFPREDEFINEDOBJECTSETUPS_MFABSTRACTSETUP_H_ */
