/*
 * MFVkPipeline.h
 *
 *  Created on: 15.07.2019
 *      Author: michl
 *
 *      TODO: fallback/default pipeline.
 *      TODO: resource/pipeline setup manager for playing with settings.
 */

#ifndef MFVULKAN_MFVKPIPELINE_H_
#define MFVULKAN_MFVKPIPELINE_H_

class MFVkPipeline;

#include <vector>
#include <cstring>
#include "../MFVkObjects/MFVkStructs.h"
#include "../MFVkObjects/MFVkShader.h"
#include "../MFVkContext/MFVkContext.h"
#include "vulkan/vulkan.h"

class MFVkPipeline {

protected:
	MFVkContext*
		mp_context;
	PipelineSetup
		*mp_pipelineSetup;

	std::vector<VkPipelineShaderStageCreateInfo>
		m_vecShaderStages;

	VkPipeline*
		mp_graphicsPipeline;

	size_t
		m_shaderStageCount=0;

	VkPipelineVertexInputStateCreateInfo* mp_vertexInputState;
	VkPipelineInputAssemblyStateCreateInfo* mp_inputAssemblyState;
	VkPipelineViewportStateCreateInfo *mp_viewportState;
	VkPipelineRasterizationStateCreateInfo* mp_rasterizationState;
	VkPipelineMultisampleStateCreateInfo* mp_multisampleState;
	VkPipelineDepthStencilStateCreateInfo* mp_depthStencilState;
	VkPipelineColorBlendStateCreateInfo* mp_colorBlendState;
	VkGraphicsPipelineCreateInfo* mp_pipelineCreateInfo;

	VkPipelineLayoutCreateInfo* mp_layoutInfo;
	VkPipelineLayout* mp_layout;

	VkPipelineColorBlendAttachmentState* mp_colorBlendAttachmentState;

	bool
		m_customShaderStage,
		m_customVertexInputState,
		m_customInputAssembly,
		m_customViewportState,
		m_customRasterizationState,
		m_customMultisampleState,
		m_customDepthStencilState,
		m_customColorBlendAttachment,
		m_customColorBlendState,
		m_customLayoutInfo,
		m_customPipelineInfo,
		m_resourcesInitiliced;

	void init();
public:
	MFVkPipeline(MFVkContext* pContext,PipelineSetup* pipelineSetup);
	MFVkPipeline(MFVkContext* pContext);
	virtual ~MFVkPipeline();
	void destroyVulkanResources();

	/**
	 * The must be initialiced before the initPipeline() call. To configure the resources, use the getter methods after calling initPipelineResources()!
	 * @return true if it was successfull else false. If false is returned you must not call initPipeline()!
	 * TODO init according to MFVkSetup fields and fallback values!
	 */
	bool initPipelineResources();


	/**
	 * This function inits the pipeline with the given resources. It will create the
	 * pipeline layout and the pipeline.
	 * @return true if successful
	 */
	bool initPipeline();
	bool createPipelineLayout();
	bool createPipeline();
	VkPipelineLayout* getPipelineLayout(){
		return mp_layout;
	};
	VkPipeline* getPipeline(){return mp_graphicsPipeline;};
	/**
	 * Iterates through the MFVkShader objects stored in the m_fragmentShaderFiles and m_vertexShaderFiles vectors and creates the shader stage array needed for pipeline creation.
	 */
	std::vector<VkPipelineShaderStageCreateInfo> createShaderStages();
	std::vector<VkPipelineShaderStageCreateInfo>* getShaderStages(){return &m_vecShaderStages;};

	VkPipelineVertexInputStateCreateInfo* createVertexInputState();
	VkPipelineVertexInputStateCreateInfo* getVertexInputState(){return mp_vertexInputState;};

	/**
	 * Creates the input assembly state.  https://vulkan-tutorial.com/Drawing_a_triangle/Graphics_pipeline_basics/Introduction :
	 * "The input assembler collects the raw vertex data from the buffers you specify and may also use an index buffer to repeat certain elements without having to duplicate the vertex data itself."
	 * @return
	 */
	VkPipelineInputAssemblyStateCreateInfo* createInputAssemblyState();
	VkPipelineInputAssemblyStateCreateInfo* getInputAssemblyState(){return mp_inputAssemblyState;};

	VkPipelineViewportStateCreateInfo* createViewportState();
	VkPipelineViewportStateCreateInfo* getViewportState(){return mp_viewportState;};

	VkPipelineRasterizationStateCreateInfo* createRasterizationState();
	VkPipelineRasterizationStateCreateInfo* getRasterizationState(){return mp_rasterizationState;};

	VkPipelineMultisampleStateCreateInfo* createMultisampleState();
	VkPipelineMultisampleStateCreateInfo* getMultisampleState(){return mp_multisampleState;};

	VkPipelineDepthStencilStateCreateInfo* createDepthStencilState();
	VkPipelineDepthStencilStateCreateInfo* getDepthStencilState(){return mp_depthStencilState;};

	VkPipelineColorBlendStateCreateInfo* createColorBlendState();
	VkPipelineColorBlendStateCreateInfo* getColorBlendState(){return mp_colorBlendState;};

	VkPipelineLayoutCreateInfo* createLayoutInfo();

	VkPipelineColorBlendAttachmentState* createColorBlendAttachmentState();
	VkPipelineColorBlendAttachmentState* getColorBlendAttachmentState(){return mp_colorBlendAttachmentState;};

	VkGraphicsPipelineCreateInfo* createPipelineInfo();

	/**
	 * Returns the pipeline setup. This can be usefull if a setup needs to be changed
	 * to specify a custom pipeline.
	 * @return
	 */
	PipelineSetup* getPipelineSetup();


};

#endif /* MFVULKAN_MFVKPIPELINE_H_ */
