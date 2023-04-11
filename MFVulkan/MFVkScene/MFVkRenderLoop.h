/*
 * MFVkRenderLoop.h
 *
 *  Created on: 11.09.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKSCENEMANAGER_MFVKRENDERLOOP_H_
#define MFVULKAN_MFVKSCENEMANAGER_MFVKRENDERLOOP_H_
#include <vector>
#include <mutex>
#include <vulkan/vulkan.h>
#include "../MFVkObjects/MFVkBuffer.h"
#include "../MFVkContext/MFVkContext.h"
#include "../MFVkResources/MFVkResources.h"
#include "../MFVkSceneManager/MFVkObjectManager.h"
#include "../MFPredefinedObjectSetups/MFAbstractSetup.h"

#include "../MFVkObjects/MFVkStructs.h"
//
//#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../MFVkSceneObject/MFVkSceneObject.h"

class MFVkRenderLoop {
private:

	std::mutex
		lockDraw;
	MFVkContext
		*mp_context;
	MFVkResources
		*mp_resources;

	std::vector<MFAbstractSetup*>
		*mp_vecObjectSetups;

	MFVkObjectManager
		*mp_objectManager;
	MFVkBuffer
		*mp_viewBuffer=nullptr;

	std::vector<MFVkSceneObject*>
		*mp_vecSceneObject;

	VkDevice
		*mp_device;

	uint32_t
		m_framesInFlight=1,
		m_currentFrame=0,
		m_currentSceneObjectCount=0,
		m_index=0;

	/*syncs*/
	std::vector<VkSemaphore>
		m_renderFinishedSemaphores,
		m_imageAvailableSemaphores;
	std::vector<VkFence>
		m_inFlightFences,
		m_vecImageInFlight;

	VkSwapchainKHR
		*mp_swapChain;
	VkExtent2D
		*mp_swapChainExtent;

	VkRenderPass
		*mp_renderPass;
	std::vector<VkFramebuffer*>
		*mp_vecFramebuffer;

	VkRenderPassBeginInfo
		*mp_renderPassInfo;
	std::vector<VkClearValue>
		*mp_clearValues;

	glm::mat4x4
		m_cameraPosition;

	std::vector<VkCommandBuffer>
		*mp_vecCommandBuffer;

	bool
		m_updateCommandBuffer;

	std::mutex
		lockUpdateTrigger;
	static std::mutex
		sLockVecRenderLoop;
	static std::vector<MFVkRenderLoop*>
		*mps_vecRenderLoops;
	static uint32_t
		ms_indexCounter;

	/*drawFrame() resources*/
	VkResult
	m_resultDrawFrame;
  uint32_t
  m_nextImgIndex=0;
  VkPresentInfoKHR
  m_presentInfo;
  VkSubmitInfo
  m_submitInfo;
  VkPipelineStageFlags
  m_waitStage;

public:
	MFVkRenderLoop(MFVkContext* pContext,MFVkResources *pResources);
	virtual ~MFVkRenderLoop();
	static void windowCloseCallback();
	static void windowResizeCallback();
	void reinitRenderLoop();
	void closeWindow();
	void resizeWindow();
	glm::mat4x4& getCameraPosition(){return m_cameraPosition;};
	MFVkBuffer* getViewBuffer(){return mp_viewBuffer;};
	//TODO addViewBuffer(MFVkBuffer*,vec<MFVkSceneObject*>* subElements);
	void setCameraPosition(glm::mat4x4 camMatrix){m_cameraPosition=camMatrix;};
	bool initRenderLoop();
	glm::mat4x4 calcPerspective(
			float fov,float aspectRation,float nearPlane,float farPlane);
	bool renderLoop();
	bool renderLoopStep();
	void drawFrame();
	/**
	 * Function which updates the command buffer, to make changes of the scene visible.
	 * @return
	 */
	bool updateCommandBuffer();

	/**
	 * Thread safe update trigger. Scedules an update of the command buffer.
	 * The command buffer will be updated after the current scene was rendered.
	 * @return
	 */
	void triggerCommandBufferUpdate();

	void setupCameraDescriptor();

	void setFramesInFlight(uint32_t count);

	bool setupCommandBuffer(VkCommandBuffer* pBuffer,VkFramebuffer* pFramebuffer,uint32_t index);
};

#endif /* MFVULKAN_MFVKSCENEMANAGER_MFVKRENDERLOOP_H_ */
