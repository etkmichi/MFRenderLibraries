#ifndef MFVULKAN_MFVKOBJECTS_MFVKSTRUCTS_H_
#define MFVULKAN_MFVKOBJECTS_MFVKSTRUCTS_H_

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <MFData/MFDataObject.h>
#include <mutex>
#include "vulkan/vulkan.h"


//#include "MFVkShader.h"
//#include "../MFVkResources/MFVkPipeline.h"
struct UBOMVPMatrix4x4 {
    alignas(16) glm::mat4 model=glm::mat4(1.0f);
    alignas(16) glm::mat4 view=glm::mat4(1.0f);
    alignas(16) glm::mat4 projection=glm::mat4(1.0f);
};

struct UBOVec4 {
	glm::vec4 data=glm::vec4(1.0f,1.0f,1.0f,1.0f);//TODO alignas??
};

struct S_UniformBuffer{
	std::vector<void*> *pVecBuffers=nullptr;
	std::vector<MFDataObject> *pVecUBOSetup=nullptr;
	void *pSceneObject=nullptr;
	uint32_t index=0xFFFFFFFF;
	void init(){
		if(pVecBuffers==nullptr)
			pVecBuffers=new std::vector<void*>();
	}
	void destroyResources(){
		delete pVecBuffers;
	}
	void addBuffer(void* pBuf){
		if(pVecBuffers==nullptr)init();
		pVecBuffers->push_back(pBuf);
	}
};

struct UBOBufferContainer{
	std::mutex lockBuffer;
	std::mutex lockVecChilds;
	std::mutex lockParent;
	UBOMVPMatrix4x4* pData;//TODO rename to pData maybe use void* and dataSize
	UBOBufferContainer* pParentContainer=nullptr;
	std::vector<UBOBufferContainer*> vecChildren;
	uint64_t index;
	VkDescriptorSet* pDescriptorSet=nullptr;
	void *pVkBuffer=nullptr;
	uint64_t descriptorIndex;
};

/**
 * This struct contains ubo's and can be used for synchronisation and buffer management.
 */
struct UBOBuffers{
	std::mutex
		lockVecUBOBuffer;
	std::vector<UBOMVPMatrix4x4>
		*vecUniformBuffers;
	uint64_t
		maxValidUBOIndex=0,
		maxUBOCount=1000;
};

struct DescriptorLayoutSetup {
	std::vector<VkDescriptorSetLayoutBinding> descriptorBindings;
};

struct PipelineSetup{
	bool useCustomShader;
	void* shaderVertexPointer;
	uint32_t shaderVertCount;
	void* shaderFragmentPointer;
	uint32_t shaderFragCount;

	bool useCustomBindings;
	VkVertexInputBindingDescription *bindingDescriptionsPointer;
	uint32_t bindingDescCount;
	VkVertexInputAttributeDescription *bindingAttributeDescPointer;
	uint32_t bindingsAttributeDescCount;

	bool useCustomAssembly;
	VkPrimitiveTopology AssemblyTopology=VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	VkBool32 AssemblyPrimitiveRestartEnable = VK_FALSE;

	bool useCustomViewport;
	VkViewport* viewportsPointer;
	uint32_t viewportCount;
	VkRect2D* viewportScissors;
	uint32_t viewportScissorCount;

	bool useCustomRasterization;
	VkBool32 RasterizationDepthClampEnable = VK_FALSE;
	VkBool32 RasterizationRasterizerDiscarEnable = VK_FALSE;
	VkBool32 RasterizationDepthBiasEnable = VK_FALSE;
	VkPolygonMode RasterizationPolygonMode = VK_POLYGON_MODE_FILL;
	float RasterizationLineWidth = 1.0f;
	VkCullModeFlags RasterizationCullMode = VK_CULL_MODE_NONE;
	VkFrontFace RasterizationFrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

	bool useCustomMultisample;
	VkBool32 MultisampleSampleShadingEnabled=VK_FALSE;
	VkSampleCountFlagBits MultisampleRasterizationSamples=VK_SAMPLE_COUNT_1_BIT;

	bool useCustomDepthStencil;
	VkBool32 depthTestEnable = VK_FALSE;
	VkBool32 depthWriteEnable = VK_FALSE;
	VkBool32 depthBoundsTestEnable = VK_FALSE;
	VkBool32 depthStencilTestEnable = VK_FALSE;
	VkCompareOp depthCompareOp = VK_COMPARE_OP_LESS;

	bool useCustomColorBlendAttachment;
	VkColorComponentFlags ColorBlendAttachmentColorWriteMask =
			VK_COLOR_COMPONENT_R_BIT |
			VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;
	VkBool32 ColorBlendAttachmentBlendEnable = VK_FALSE;

	bool useCustomColorBlending;
	VkBool32 ColorBlendinglogicOpEnable = VK_FALSE;
	VkLogicOp ColorBlendinglogicOp = VK_LOGIC_OP_COPY;
	uint32_t ColorBlendingAttachmentCount = 1;
	VkPipelineColorBlendAttachmentState* ColorBlendingAttachmentsPointer = nullptr;
	float ColorBlendingBlendConstant1 = 0.0f;
	float ColorBlendingBlendConstant2 = 0.0f;
	float ColorBlendingBlendConstant3 = 0.0f;
	float ColorBlendingBlendConstant4 = 0.0f;

	bool useCustomDescriptorLayouts;
	VkDescriptorSetLayout* DescriptorSetLayoutPointer;
	uint32_t DescriptorSetLayoutCount;

	bool useCustomLayout;
	VkPipelineLayout* pPipelineLayout;

	bool useCustomRenderpass;
	VkRenderPass *pRenderPass;

	bool useCustomDynamicSetup;
	uint32_t dynamicStateCount;
	VkPipelineDynamicStateCreateFlags dynamicStateFlags;
	const VkDynamicState* dynamicStatesPointer;

	bool useCustomTesselation;
	VkPipelineTessellationStateCreateFlags tesselationStateFlags;
	uint32_t tesselationPatchControlPoints;
};

struct PipelineContext{
	void* pipeline;
	bool destroyInManager=false;
	std::string pipeDescription;/*short description of usage*/
	uint32_t index;
};

struct SimpleVertex{
	glm::vec3 pos={0,0,0};

	static VkVertexInputBindingDescription* getBindingDescription() {
		VkVertexInputBindingDescription *bindingDescription =
				new VkVertexInputBindingDescription();//TODO delete
		bindingDescription->binding = 0;
		bindingDescription->stride = sizeof(glm::vec3);
		bindingDescription->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 1>*
											getAttributeDescriptions() {//TODO delete
		std::array<VkVertexInputAttributeDescription,1> *attributeDescriptions
		= new std::array<VkVertexInputAttributeDescription,1>();

		attributeDescriptions->at(0).binding = 0;
		attributeDescriptions->at(0).location = 0;
		attributeDescriptions->at(0).format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions->at(0).offset = 0;

		return attributeDescriptions;
	}

	bool operator==(const SimpleVertex& other) const {
		return pos == other.pos;
	}
};

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription* getBindingDescription() {
        VkVertexInputBindingDescription *bindingDescription = new VkVertexInputBindingDescription();
        bindingDescription->binding = 0;
        bindingDescription->stride = sizeof(Vertex);
        bindingDescription->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }

    bool operator==(const Vertex& other) const {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }
};
#endif
