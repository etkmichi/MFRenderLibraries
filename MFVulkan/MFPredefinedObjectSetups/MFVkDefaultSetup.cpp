/*
 * MFVkDefaultSetup.cpp
 *
 *  Created on: 11.12.2019
 *      Author: michl
 */

#include "MFVkDefaultSetup.h"
#include <MFObjects/MFObject.h>
#include <MFBaseGeometries/MFGeometryData.h>

UBOMVPMatrix4x4* dummyMVP=new UBOMVPMatrix4x4();
glm::vec4 *dummyColor=new glm::vec4(1,1,0,1);
MFDataObject mvpStructure,colorStructure;
MFDataObject modelMat,viewMat,perspectiveMat;


MFVkDefaultSetup::MFVkDefaultSetup(MFVkContext* pContext, MFVkResources *pResources):
	MFAbstractSetup(pContext, pResources)
{
	mp_defaultShader=nullptr;
	dummyMVP->model=glm::translate(glm::mat4x4(1.0), glm::vec3(0,0,0));
	dummyMVP->view = glm::lookAt(
			glm::vec3(2.0f, 2.0f, 2.0f), //Position of cam (eye)
			glm::vec3(0.0f, 0.0f, 0.0f), //look at this position (center)
			glm::vec3(0.0f, 0.0f, 1.0f));//upward direction (up);
	dummyMVP->projection=calcPerspective(75.0f, 0.75f, 0.1, 201.0f);
	mvpStructure.setDataByteSize(sizeof(UBOMVPMatrix4x4));
	mvpStructure.setInitialData(dummyMVP);

	modelMat.setDataByteSize(sizeof(glm::mat4x4));
	modelMat.setInitialData(&dummyMVP->model);

	viewMat.setDataByteSize(sizeof(glm::mat4x4));
	viewMat.setInitialData(&dummyMVP->view);
	viewMat.setStatic(true);

	perspectiveMat.setDataByteSize(sizeof(glm::mat4x4));
	perspectiveMat.setInitialData(&dummyMVP->projection);
	perspectiveMat.setStatic(true);

	colorStructure.setDataByteSize(sizeof(glm::vec4));
	colorStructure.setInitialData(dummyColor);
}

MFVkDefaultSetup::~MFVkDefaultSetup() {
}

glm::mat4x4 MFVkDefaultSetup::calcPerspective(float fov,float aspectRation,float nearPlane,float farPlane){
	glm::mat4x4 mat={};
	float c= 1.0f / tan(((1.0f*3.1415*fov)/360));
	mat[0][0]=c/aspectRation;
	mat[0][1]=.0f;
	mat[0][2]=.0f;
	mat[0][3]=.0f;

	mat[1][0]=.0f;
	mat[1][1]=-c;
	mat[1][2]=.0f;
	mat[1][3]=.0f;

	mat[2][0]=.0f;
	mat[2][1]=.0f;
	mat[2][2]=farPlane/(nearPlane-farPlane);
	mat[2][3]=-1.0f;

	mat[3][0]=.0f;
	mat[3][1]=.0f;
	mat[3][2]=(nearPlane*farPlane)/(nearPlane-farPlane);
	mat[3][3]=.0f;
	return mat;
}

/*UBO manager functions*/
/*Object functions*/
bool MFVkDefaultSetup::setupUBOManager(MFVkUBOManager* pUBOManager){
	//pUBOManager->addDataType(&mvpStructure);//Transformation
	//TODO split uniform matrices into one uniform for each matrix
	//set to matrix to be static!
	pUBOManager->addDataType(&modelMat);
	pUBOManager->addDataType(&viewMat);
	pUBOManager->addDataType(&perspectiveMat);
	pUBOManager->addDataType(&colorStructure);//Color
	return true;
}

MFVkBuffer* MFVkDefaultSetup::getViewBuffer(){
	MFVkBuffer* pViewBuffer=nullptr;
	if(isUBOManagerCreated){
		pViewBuffer=(MFVkBuffer*)(mp_uboManager->getUBOBuffer(0)->pVecBuffers->at(1));
	}
	return pViewBuffer;
}

bool MFVkDefaultSetup::setupSOUniformCollection(
		MFVkSceneObjectBuffers* pSOBuffers,
		S_UniformBuffer* ubo,
		MFVkSceneObject* pSceneObject){
	if(pSOBuffers == nullptr || ubo==nullptr || ubo->pVecBuffers==nullptr){
		mp_context->printErr("MFVkDefaultSetup::setupSceneObjectBufferCollection - "
				"pSOBuffers == nullptr || ubo==nullptr || ubo->pVecBuffers==nullptr");
		return false;
	}
	bool ret=true;
	if(ubo->pVecBuffers->at(0)==nullptr){
		mp_context->printErr("MFVkDefaultSetup::setupSceneObjectBufferCollection - "
				"ubo->pVecBuffers->at(0)==nullptr");
		ret=false;
	}else{
		pSOBuffers->setModelMatrix((MFVkBuffer*)ubo->pVecBuffers->at(0));
	}
	if(ubo->pVecBuffers->at(1)==nullptr){
		mp_context->printErr("MFVkDefaultSetup::setupSceneObjectBufferCollection - "
				"ubo->pVecBuffers->at(1)==nullptr");
		ret=false;
	}else{
		pSOBuffers->setViewMatrix((MFVkBuffer*)ubo->pVecBuffers->at(1));
	}
	if(ubo->pVecBuffers->at(2)==nullptr){
		mp_context->printErr("MFVkDefaultSetup::setupSceneObjectBufferCollection - "
				"ubo->pVecBuffers->at(2)==nullptr");
		ret=false;
	}else{
		pSOBuffers->setPerspectiveMatrix((MFVkBuffer*)ubo->pVecBuffers->at(2));
	}
	if(ubo->pVecBuffers->at(3)==nullptr){
		mp_context->printErr("MFVkDefaultSetup::setupSceneObjectBufferCollection - "
				"ubo->pVecBuffers->at(3)==nullptr");
		ret=false;
	}else{
		pSOBuffers->setColor((MFVkBuffer*)ubo->pVecBuffers->at(3));
		pSOBuffers->getColor()->setInData(pSceneObject->getColorBuffer());
		pSOBuffers->getColor()->updateBuffer();
	}
	return ret;
}

bool MFVkDefaultSetup::reinitUBOManager(MFVkUBOManager* uboManager){
	/*Overwrite the perspective matrix*/
	//2 -> see setupUBOManager - perspective is added with index 2
	MFVkBuffer* pPers=(MFVkBuffer*)uboManager->getUBOBuffer(0)->pVecBuffers->at(2);
	void* dst=pPers->getInData();
	VkExtent2D extent=mp_resources->getSwapChainExtent();
	float aspectRatio=(1.0f*extent.width)/(1.0f*extent.height);
	dummyMVP->projection=calcPerspective(
			75.0f,
			aspectRatio,
			0.1f,
			201.0f);
	memcpy(dst,&dummyMVP->projection,pPers->getSize());
	return uboManager->recreateBuffers();
}

/*Shader functions*/
bool MFVkDefaultSetup::initShader(){
	if(mp_defaultShader==nullptr)
		mp_defaultShader=new MFVkDefaultShader(mp_context);
	if(!mp_defaultShader->initDefaultShader())
		return false;
	mp_vertexShader=mp_defaultShader->getVertexShader();
	mp_fragmentShader=mp_defaultShader->getFragmentShader();
	return true;
}

bool MFVkDefaultSetup::createVertexShader(MFVkShader* pVertexShader){
	return mp_defaultShader->initDefaultVertexShader(pVertexShader);
}

bool MFVkDefaultSetup::createFragmentShader(MFVkShader* pFragmentShader){
	return mp_defaultShader->initDefaultFragmentShader(pFragmentShader);
}

bool MFVkDefaultSetup::createDescriptorWrites(std::vector<VkWriteDescriptorSet>* pWrites){
	pWrites->resize(4);
	pWrites->at(0).sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	pWrites->at(0).dstBinding = 0;
	pWrites->at(0).dstArrayElement = 0;
	pWrites->at(0).descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pWrites->at(0).descriptorCount = 1;
	pWrites->at(0).dstSet = VK_NULL_HANDLE;
	pWrites->at(0).pBufferInfo = nullptr;

	pWrites->at(1).sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	pWrites->at(1).dstBinding = 1;
	pWrites->at(1).dstArrayElement = 0;
	pWrites->at(1).descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pWrites->at(1).descriptorCount = 1;
	pWrites->at(1).dstSet = VK_NULL_HANDLE;
	pWrites->at(1).pBufferInfo = nullptr;

	pWrites->at(2).sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	pWrites->at(2).dstBinding = 2;
	pWrites->at(2).dstArrayElement = 0;
	pWrites->at(2).descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pWrites->at(2).descriptorCount = 1;
	pWrites->at(2).dstSet = VK_NULL_HANDLE;
	pWrites->at(2).pBufferInfo = nullptr;

	pWrites->at(3).sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	pWrites->at(3).dstBinding = 3;
	pWrites->at(3).dstArrayElement = 0;
	pWrites->at(3).descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pWrites->at(3).descriptorCount = 1;
	pWrites->at(3).dstSet = VK_NULL_HANDLE;
	pWrites->at(3).pBufferInfo = nullptr;
	return true;
}

bool MFVkDefaultSetup::setupRenderCommand(
		VkCommandBuffer* pBuffer,
		MFVkSceneObject* pObject){
	//mp_context->printInfo("MFVkRenderLoop::setupCommandBuffer adding scene object render commands!");
	vkCmdBindPipeline(
		*pBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		*mp_pipeline->getPipeline());

	vkCmdBindDescriptorSets(
		*pBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		*mp_pipeline->getPipelineLayout(),
		0,
		1,
		pObject->getDescriptorSet(),
		0,
		nullptr);
	//bind drawing pipeline of object
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(
		*pBuffer,
		0,
		1,
		pObject->getVertexBuffer()->getBuffer(),
		offsets);
	vkCmdBindIndexBuffer(
		*pBuffer,
		*pObject->getIndexBuffer()->getBuffer(),
		0,
		VK_INDEX_TYPE_UINT32);

	//draw data
	vkCmdDrawIndexed(
		*pBuffer,
		pObject->getIndicesCount() , 1, 0, 0, 0);

	return true;
};

bool MFVkDefaultSetup::createDescriptorLayoutBindings(
		std::vector<VkDescriptorSetLayoutBinding>* pVecBindings){
	pVecBindings->resize(4);
	(*pVecBindings)[0].binding=0;
	(*pVecBindings)[0].descriptorCount=1;
	(*pVecBindings)[0].descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	(*pVecBindings)[0].pImmutableSamplers=nullptr;
	(*pVecBindings)[0].stageFlags=VK_SHADER_STAGE_VERTEX_BIT;

	(*pVecBindings)[1].binding=1;
	(*pVecBindings)[1].descriptorCount=1;
	(*pVecBindings)[1].descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	(*pVecBindings)[1].pImmutableSamplers=nullptr;
	(*pVecBindings)[1].stageFlags=VK_SHADER_STAGE_VERTEX_BIT;

	(*pVecBindings)[2].binding=2;
	(*pVecBindings)[2].descriptorCount=1;
	(*pVecBindings)[2].descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	(*pVecBindings)[2].pImmutableSamplers=nullptr;
	(*pVecBindings)[2].stageFlags=VK_SHADER_STAGE_VERTEX_BIT;

	(*pVecBindings)[3].binding=3;//color=?=
	(*pVecBindings)[3].descriptorCount=1;
	(*pVecBindings)[3].descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	(*pVecBindings)[3].pImmutableSamplers=nullptr;
	(*pVecBindings)[3].stageFlags=VK_SHADER_STAGE_FRAGMENT_BIT;
	return true;
}

bool MFVkDefaultSetup::createDescriptorSetLayout(
		VkDescriptorSetLayout *pDescriptorSetLayout){
	VkDescriptorSetLayoutCreateInfo info= {};
	if(pDescriptorSetLayout==nullptr){
		pDescriptorSetLayout=new VkDescriptorSetLayout();
	}
	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	info.bindingCount=static_cast<uint32_t>(
			mp_vecDescriptorBinding->size());
	info.pBindings =
			mp_vecDescriptorBinding->data();

	if(VK_SUCCESS!=vkCreateDescriptorSetLayout(
			*mp_context->getVirtualDevice(),
			&info,
			nullptr,
			pDescriptorSetLayout)){
		mp_context->printErr("MFVkDescriptorManager::createDefaultLayout() - failed!!");
		return false;
	}
	mp_context->printInfo("MFVkDescriptorManager::createDefaultLayout() - done!");
	return true;
}

bool MFVkDefaultSetup::createDescriptorPool(VkDescriptorPool *pPool){
	std::array<VkDescriptorPoolSize, 4> poolSizesObjects = {};

	/*MVP*/
	poolSizesObjects[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizesObjects[0].descriptorCount = m_objectCount;

	//TODO test what happens if descriptorCount is set to 1
		//- just one desc is needed for a static value?!?
	poolSizesObjects[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizesObjects[1].descriptorCount = m_objectCount;

	poolSizesObjects[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizesObjects[2].descriptorCount = m_objectCount;

	/*Color*/
	poolSizesObjects[3].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizesObjects[3].descriptorCount = m_objectCount;

	VkDescriptorPoolCreateInfo objectsPoolInfo = {};
	objectsPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	objectsPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizesObjects.size());
	objectsPoolInfo.pPoolSizes = poolSizesObjects.data();
	objectsPoolInfo.maxSets = static_cast<uint32_t>(m_objectCount);

	if (vkCreateDescriptorPool(
			*mp_context->getVirtualDevice(),
			&objectsPoolInfo,
			nullptr,
			pPool) != VK_SUCCESS) {
		mp_context->printErr("MFVkDescriptorManager - createDefaultPool - "
				"failed to create descriptor pool for objects!");
		return false;
	}
	return true;
}

MFVkVertexDataCollection* MFVkDefaultSetup::createDataCollection(
		MFVkSceneObject* pObject){
	MFGeometryData *pGeoDat=&pObject->getGeometry()->getGeometryData();
	MFVkVertexDataCollection* pCollection=nullptr;
	if(pGeoDat->m_creationFlag==E_CREATE_BY_COLLECTION){
		/*collection is created by geometry, take it.*/
		pCollection=new MFVkVertexDataCollection(
				pObject->getGeometry()->getVertexDataCollection(),
				mp_context,
				mp_resources);
	}
	if(pCollection==nullptr){
		/*collection will be created by code of MFVkVertex...*/
		pCollection=new MFVkVertexDataCollection(
				mp_context,mp_resources);
		pCollection->setGeometryData(pGeoDat);
		pCollection->use32BitIndexedNormals(true);
		pCollection->createCollection();
	}
	//TODO add to delete vector
	return pCollection;
}

bool MFVkDefaultSetup::createVertexModel(MFVkComplexVertex* pModel){
	pModel->add3DVec(nullptr);
	uint32_t* pIndex=nullptr;
	pModel->addData(pIndex);
	return true;
}

/*pipeline*/
void MFVkDefaultSetup::createPipelineSetup(PipelineSetup* setup){
	setup->useCustomShader=true;
	setup->shaderVertCount = 1;
	setup->shaderFragCount = 1;
	setup->shaderVertexPointer = mp_vertexShader;
	setup->shaderFragmentPointer = mp_fragmentShader;
	MFVkComplexVertex* pDummyVert=mp_vertexModel;
	setup->useCustomBindings=true;
	std::vector<VkVertexInputAttributeDescription>* pAttrDesc=
			pDummyVert->createAttributeDescription();
	VkVertexInputBindingDescription* pVIBD=pDummyVert->createBindingDescription();
	setup->bindingDescriptionsPointer =pVIBD;
	setup->bindingDescCount=1;
	setup->bindingAttributeDescPointer =pAttrDesc->data();
	setup->bindingsAttributeDescCount =pAttrDesc->size();

	setup->useCustomAssembly=true;
	setup->AssemblyTopology=VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	setup->AssemblyPrimitiveRestartEnable=VK_FALSE;

	if(setup->viewportsPointer==nullptr){
		VkViewport *pViewport = new VkViewport();//TODO delete
		setup->viewportsPointer = pViewport;
	}
	setup->viewportsPointer->x = 0.0f;
	setup->viewportsPointer->y = 0.0f;
	setup->viewportsPointer->width =
			(float) mp_resources->getSwapchain()->getExtent().width;
	setup->viewportsPointer->height =
			(float) mp_resources->getSwapchain()->getExtent().height;
	setup->viewportsPointer->minDepth = 0.1f;
	setup->viewportsPointer->maxDepth = 0.5f;

	if(setup->viewportScissors==nullptr){
		VkRect2D *pScissor = new VkRect2D();//TODO delete
		setup->viewportScissors = pScissor;
	}

	setup->viewportScissors->offset = {0, 0};
	setup->viewportScissors->extent = mp_resources->getSwapchain()->getExtent();

	setup->useCustomViewport=true;
	setup->viewportCount = 1;
	setup->viewportScissorCount = 1;

	setup->useCustomRasterization = true;
	setup->RasterizationDepthClampEnable = VK_FALSE;
	setup->RasterizationRasterizerDiscarEnable = VK_FALSE;
	setup->RasterizationDepthBiasEnable = VK_FALSE;
	setup->RasterizationPolygonMode = VK_POLYGON_MODE_FILL;
	setup->RasterizationLineWidth = 1.0f;
	setup->RasterizationCullMode = VK_CULL_MODE_NONE ;//VK_CULL_MODE_NONE;
	setup->RasterizationFrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

	setup->useCustomMultisample=true;
	setup->MultisampleSampleShadingEnabled=VK_FALSE;
	setup->MultisampleRasterizationSamples=mp_context->getSetup()->m_msaaSamples;

	setup->useCustomDepthStencil=true;
	setup->depthTestEnable = VK_TRUE;//VK_TRUE
	setup->depthWriteEnable = VK_TRUE;
	setup->depthBoundsTestEnable = VK_FALSE;
	setup->depthStencilTestEnable = VK_FALSE;
	setup->depthCompareOp = VK_COMPARE_OP_LESS;

	setup->useCustomColorBlendAttachment=true;
	setup->ColorBlendAttachmentColorWriteMask =
			VK_COLOR_COMPONENT_R_BIT |
			VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;
	setup->ColorBlendAttachmentBlendEnable = VK_FALSE;

	setup->useCustomColorBlending=true;
	setup->ColorBlendinglogicOpEnable = VK_FALSE;
	setup->ColorBlendinglogicOp = VK_LOGIC_OP_COPY;
	setup->ColorBlendingAttachmentCount = 1;
	setup->ColorBlendingAttachmentsPointer = nullptr;//TODO this is pipeline intern!
	setup->ColorBlendingBlendConstant1 = 0.0f;
	setup->ColorBlendingBlendConstant2 = 0.0f;
	setup->ColorBlendingBlendConstant3 = 0.0f;
	setup->ColorBlendingBlendConstant4 = 0.0f;

	setup->useCustomDescriptorLayouts=true;
	setup->DescriptorSetLayoutPointer=mp_descriptorSetLayout;//{*mp_defaultDescriptorSetLayout};//new std::vector<>(1);
	setup->DescriptorSetLayoutCount=1;

	setup->useCustomLayout=false;/*will be created my MFVkPipeline*/
	setup->pPipelineLayout=nullptr;

	setup->useCustomDynamicSetup=true;
	setup->dynamicStateCount=0;
	setup->dynamicStateFlags=0;
	setup->dynamicStatesPointer=nullptr;

	setup->useCustomTesselation=true;
	setup->tesselationStateFlags=0;
	setup->tesselationPatchControlPoints=0;
}

void MFVkDefaultSetup::printObjectColor(MFVkSceneObject* object){
	UBOVec4 color=*object->getColorBuffer();
	mp_context->printInfo("Color r/g/b/a: "+
		std::to_string(color.data.x)+"/"+
		std::to_string(color.data.y)+"/"+
		std::to_string(color.data.z)+"/"+
		std::to_string(color.data.w));
}
