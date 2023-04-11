/*
 * MFVkDefaultSetup.h
 *
 *  Created on: 11.12.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFPREDEFINEDOBJECTSETUPS_MFVKDEFAULTSETUP_H_
#define MFVULKAN_MFPREDEFINEDOBJECTSETUPS_MFVKDEFAULTSETUP_H_
class MFVkDefaultSetup;
#include "MFAbstractSetup.h"
#include "../MFVkObjectComponents/MFVkPipeline.h"
#include "../MFVkPredefinedShaders/MFVkDefaultShader.h"
#include "../MFVkSceneManager/MFVkUBOManager.h"
#include "../MFVkVertex/MFVkComplexVertex.h"
#include "../MFVkVertex/MFVkVertexDataCollection.h"
#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

class MFVkDefaultSetup : public MFAbstractSetup {
private:
	MFVkDefaultShader
		*mp_defaultShader;

public:
	MFVkDefaultSetup(MFVkContext* pContext, MFVkResources *pResources);
	virtual ~MFVkDefaultSetup();

	bool setupUBOManager(MFVkUBOManager* pUBOManager);

	bool initShader();
	bool reinitUBOManager(MFVkUBOManager* uboManager);
	MFVkVertexDataCollection* createDataCollection(MFVkSceneObject* object);
	bool createVertexModel(MFVkComplexVertex* pModel);
	bool createVertexShader(MFVkShader* pVertexShader);
	bool createFragmentShader(MFVkShader* pFragmentShader);
	bool createDescriptorSetLayout(VkDescriptorSetLayout *pDescriptorSetLayout);
	bool createDescriptorPool(VkDescriptorPool *pPool);
	bool createDescriptorLayoutBindings(
			std::vector<VkDescriptorSetLayoutBinding>* pVecBindings);
	bool createDescriptorWrites(std::vector<VkWriteDescriptorSet>* pWrites);
	void createPipelineSetup(PipelineSetup* setup);

	MFVkBuffer* getViewBuffer();
	bool setupSOUniformCollection(
				MFVkSceneObjectBuffers* pSOBuffers,
				S_UniformBuffer* ubo,
				MFVkSceneObject* pSceneObject);
	bool setupRenderCommand(VkCommandBuffer* pBuffer,MFVkSceneObject* pObject);

	void printObjectColor(MFVkSceneObject* object);
	glm::mat4x4 calcPerspective(
			float fov,float aspectRation,float nearPlane,float farPlane);

};

#endif /* MFVULKAN_MFPREDEFINEDOBJECTSETUPS_MFVKDEFAULTSETUP_H_ */
