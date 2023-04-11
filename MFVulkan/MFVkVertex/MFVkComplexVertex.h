/*
 * MFVkComplexVertex.h
 *
 *  Created on: 04.12.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKVERTEX_MFVKCOMPLEXVERTEX_H_
#define MFVULKAN_MFVKVERTEX_MFVKCOMPLEXVERTEX_H_
#include <vulkan/vulkan.h>
#include <MFVertexClasses/MFAdaptableVertex.h>
#include <MFData/MFDataObject.h>
#include <map>
#include "../../MFRenderContext.h"

/**
 * Take care to add the data in the right order and
 * note the order for further usage! (Wrong usage through corruption in the data layout
 * will lead to a significant failure!)
 */
class MFVkComplexVertex : public MFAdaptableVertex{
private:
  MFRenderContext
  *mp_context;

	std::map<E_DataType,VkFormat>
		*mp_mapVulkanFormats;
	uint32_t
		m_vertexSize;
	VkVertexInputBindingDescription
		*mp_bindingDescription;

	void init();
public:
	MFVkComplexVertex(MFRenderContext* pContext);
	MFVkComplexVertex(MFAdaptableVertex *pCopy,MFRenderContext* pContext);
	virtual ~MFVkComplexVertex();

	VkFormat getVulkanFormat(E_DataType type);
	uint32_t getVertexSize(){return m_vertexSize;};

	/**
	 *
	 * @return
	 */
	VkVertexInputBindingDescription* createBindingDescription();

	/**
	 * The attribute description is needed by the vulkan pipeline (MFVkPipeline and
	 * MFVkPipelineManager). This function iterates through the set up data of the
	 * underlying parent class (MFAdaptableVertex) object and creates the attrib. desc..
	 *
	 * @return
	 */
	std::vector<VkVertexInputAttributeDescription>* createAttributeDescription();
};

#endif /* MFVULKAN_MFVKVERTEX_MFVKCOMPLEXVERTEX_H_ */
