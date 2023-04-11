/*
 * MFVkInterfaceSceneObject.h
 *
 *  Created on: 27.08.2019
 *      Author: michl
 */
#ifndef MFVULKAN_MFVKSCENEOBJECT_MFVKSCENEOBJECT_H_
#define MFVULKAN_MFVKSCENEOBJECT_MFVKSCENEOBJECT_H_

class MFVkSceneObject;

#include <vulkan/vulkan.h>
#include <MFBaseGeometries/MFAbstractGeometry.h>
#include "../MFVkObjects/MFVkStructs.h"
#include "../MFVkObjectComponents/MFVkPipeline.h"
#include "../MFVkObjects/MFVkShader.h"
#include "../MFVkVertex/MFVkVertexDataCollection.h"
#include "../MFVkSceneManager/MFVkUBOManager.h"
#include "MFVkSceneObjectBuffers.h"
//TODO setup scene object -> an object may consist of children, after the setUBOBUfferPointer function was called, all the childs can be added and set up...
/**
 * This class provides handling with the management system.
 * Use the ubo field for 3d scene objects. This class will ensure
 * the objects will be managed correctly.
 */
class MFVkSceneObject {
private:
	std::string name;
	bool
		m_isDisabled=false;
	S_UniformBuffer
		*mp_uboContainer;
	MFVkSceneObjectBuffers
		*mp_sceneObjectBuffers;

	uint32_t
		m_bufferIndex=0,
		m_indicesCount=0,
		m_verticesCount=0;
	MFVkShader
		*mp_vertexShader,
		*mp_fragmentShader;
	MFVkPipeline
		*mp_pipeline;

	VkDescriptorSet
		*mp_descriptor;
	VkDescriptorSetLayout
		*mp_descriptorSetLayout;

	MFVkBuffer
		*mp_indexBuffer,
		*mp_vertexBuffer;
	UBOVec4
		*mp_colorBuffer;
	MFVkVertexDataCollection
		*mp_vertexDataCollection;
	MFAbstractGeometry
		*mp_geometry;

public:
	MFVkSceneObject();
	virtual ~MFVkSceneObject();

	/**
	 * This method must be called by the manager of the ubo buffers.
	 * The UBO buffers can be allocated by an external manager.
	 */
	void setUBOBuffer(S_UniformBuffer* pUBOContainer);
	S_UniformBuffer* getUBOBuffer();

	MFVkSceneObjectBuffers* getBufferCollection();

	void setName(std::string name){this->name=name;};
	std::string getName(){return name;};

	void setDescriptorSet(VkDescriptorSet* pDescriptor);
	VkDescriptorSet* getDescriptorSet();

	void setVertexBuffer(MFVkBuffer* vertexBuffer,uint32_t verticesCount);
	void setIndexBuffer(MFVkBuffer* indexBuffer,uint32_t indicesCount);
	uint32_t getIndicesCount(){return m_indicesCount;};
	uint32_t getVerticesCount(){return m_verticesCount;};

	void setVertexDataCollection(MFVkVertexDataCollection* pVertexDataCollection);
	MFVkVertexDataCollection* getVertexDataCollection();

	MFVkBuffer* getVertexBuffer();
	MFVkBuffer* getIndexBuffer();

	void setGeometry(MFAbstractGeometry* pGeometry){mp_geometry=pGeometry;};
	MFAbstractGeometry* getGeometry(){return mp_geometry;};

	MFVkShader* getVertexShader();
	MFVkShader* getFragmentShader();
	MFVkPipeline* getPipeline();

	void setVertexShader(MFVkShader* shader);
	void setFragmentShader(MFVkShader* shader);
	void setDescriptorSetLayout(VkDescriptorSetLayout* pLayout)
	{mp_descriptorSetLayout=pLayout;};
	void setColor(glm::vec4 color);
	UBOVec4* getColorBuffer(){return mp_colorBuffer;};
	void setDescriptorWrites(std::vector<VkWriteDescriptorSet> writes);

	void setPipeline(MFVkPipeline* pipeline);
	void disable(){m_isDisabled=true;};
	void enable(){m_isDisabled=false;};
	bool isDisabled(){return m_isDisabled;};
	bool isEnabled(){return !m_isDisabled;};
	void updateVulkanBuffer();
	//virtual MFGeometryData* getConvexHull();
};

#endif /* MFVULKAN_MFVKSCENEOBJECT_MFVKSCENEOBJECT_H_ */
