/*
 * MFVkRenderLoop.cpp
 *
 *  Created on: 11.09.2019
 *      Author: michl
 *      TODO
 *      - create render commands for each object with the added MFAbstractSetup's
 *      	setupRenderCommands function
 *      	- change default setup to use extra uniforms for perspective and view matrix
 *      		- use only one perspective and view memory for all objects!
 *      		- Test if different descriptor sets can use the same buffer.
 */

#include "MFVkRenderLoop.h"
#include "../MFVkSceneManager/MFVkUBOManager.h"
std::vector<MFVkRenderLoop*>* MFVkRenderLoop::mps_vecRenderLoops=
    new std::vector<MFVkRenderLoop*>();
MFVkRenderLoop::MFVkRenderLoop(MFVkContext* pContext,MFVkResources *pResources):
	    lockDraw(){
  mp_context=pContext;
  mp_resources=pResources;
  mp_device=nullptr;
  mp_vecSceneObject=nullptr;
  mp_swapChain=nullptr;
  mp_objectManager=nullptr;
  mp_vecFramebuffer=nullptr;
  mp_vecCommandBuffer=nullptr;
  mp_clearValues=nullptr;
  mp_renderPassInfo=nullptr;
  mp_renderPass=nullptr;
  m_cameraPosition=glm::lookAt(
      glm::vec3(1.0f, 1.0f, 1.0f), //Position of cam (eye)
      glm::vec3(0.0f, 0.0f, 0.0f), //look at this position (center)
      glm::vec3(0.0f, 0.0f, 1.0f));//upward direction (up);
  mp_swapChainExtent=new VkExtent2D();
  m_updateCommandBuffer=false;
  mp_renderPassInfo=new VkRenderPassBeginInfo();
  mp_vecObjectSetups=mp_resources->getVecObjectSetups();
  m_framesInFlight=3;
  m_currentFrame=2;
  m_presentInfo={};
  m_submitInfo = {};
  m_resultDrawFrame=VkResult::VK_SUCCESS;
  m_waitStage=VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
}

MFVkRenderLoop::~MFVkRenderLoop() {
  // TODO Auto-generated destructor stub
}

void MFVkRenderLoop::reinitRenderLoop(){
  mp_vecFramebuffer=mp_resources->getFramebuffer()->getVecFramebuffer();
  mp_device=mp_context->getVirtualDevice();
  mp_swapChain=mp_resources->getSwapchain()->getSwapChain();
  *mp_swapChainExtent=(mp_resources->getSwapchain()->getExtent());
  mp_renderPass=mp_resources->getRenderPass()->getRenderPass();
  mp_vecCommandBuffer= mp_resources->getCommandManager()->getVecCommandBuffer();

  updateCommandBuffer();
}

bool MFVkRenderLoop::initRenderLoop(){
  mp_context->printInfo("MFVkRenderLoop::initRenderLoop()");
  mp_device=mp_context->getVirtualDevice();
  mp_swapChain=mp_resources->getSwapchain()->getSwapChain();
  *mp_swapChainExtent=(mp_resources->getSwapchain()->getExtent());
  mp_renderPass=mp_resources->getRenderPass()->getRenderPass();
  /*TODO object specific render pass?*/
  mp_vecFramebuffer=mp_resources->getFramebuffer()->getVecFramebuffer();

  mp_objectManager=mp_resources->getObjectManager();

  mp_clearValues=new std::vector<VkClearValue>(2);
  mp_clearValues->at(0).color = {0.5f, 0.5f, 0.5f, 0.0f};
  mp_clearValues->at(1).color = {0.1f, 0.1f, 0.1f, 0.0f};
  mp_clearValues->at(1).depthStencil = {1.0f, 0};//TODO setup according to depth image (move to resources)

  m_renderFinishedSemaphores.resize(m_framesInFlight);
  m_imageAvailableSemaphores.resize(m_framesInFlight);
  m_inFlightFences.resize(m_framesInFlight);
  m_vecImageInFlight.resize(mp_vecFramebuffer->size(),VK_NULL_HANDLE);

  VkSemaphoreCreateInfo semaphoreInfo = {};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo = {};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  if(mp_vecObjectSetups->size()>=1){
    mp_viewBuffer=mp_vecObjectSetups->at(0)->getViewBuffer();
  }
  for (size_t i = 0; i < m_framesInFlight; i++) {
    if (vkCreateSemaphore(*mp_device,
        &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
        vkCreateSemaphore(*mp_device,
            &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(*mp_device,
                &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS) {
      mp_context->printErr("MFVkRenderLoop::initRenderLoop - "
          "failed to create sync things");
      return false;
    }
  }

  mp_vecCommandBuffer= mp_resources->getCommandManager()->getVecCommandBuffer();


  /*pre sets for drawFrame()*/
  m_presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  m_presentInfo.waitSemaphoreCount = 1;
  m_presentInfo.swapchainCount = 1;
  m_presentInfo.pSwapchains = mp_swapChain;

  /*pre sets for drawFrame()*/
  m_submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  m_submitInfo.waitSemaphoreCount = 1;
  m_submitInfo.commandBufferCount = 1;
  m_submitInfo.signalSemaphoreCount = 1;
  m_submitInfo.pWaitDstStageMask = &m_waitStage;

  bool ret = updateCommandBuffer();
  return ret;
}

glm::mat4x4 MFVkRenderLoop::calcPerspective(float fov,float aspectRation,float nearPlane,float farPlane){
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

bool MFVkRenderLoop::renderLoopStep(){
  if(mp_context->getWindowContainer()->pollWindowResize()){
    mp_resources->recreateSwapChain();
    reinitRenderLoop();
    triggerCommandBufferUpdate();
  }

  drawFrame();

  return !mp_context->getWindowContainer()->pollWindowClose();
}

bool MFVkRenderLoop::renderLoop(){
  mp_context->printInfo("MFVkRenderLoop::renderLoop()");
  try{
    //TODO implement cam update function in render loop
    while(!mp_context->getWindowContainer()->pollWindowClose()){
      if(mp_context->getWindowContainer()->pollWindowResize()){
        mp_resources->recreateSwapChain();
        reinitRenderLoop();
        triggerCommandBufferUpdate();
      }
      lockDraw.lock();
      drawFrame();
      lockDraw.unlock();
    }
    vkDeviceWaitIdle(*mp_device);
  }catch(std::exception &e){
    mp_context->printErr("MFVkRenderLoop::renderLoop() - exception cought! "+std::string(e.what()));
    return false;
  }

  mp_context->printErr("MFVkRenderLoop::renderLoop() - end reached!");
  return true;
}

void MFVkRenderLoop::drawFrame(){//TODO time measurement for fps
  vkWaitForFences(//wait for the fence, if available, the frame isnt used.
      *mp_device,
      1,
      &m_inFlightFences[m_currentFrame],
      VK_TRUE,
      std::numeric_limits<uint64_t>::max());

  //TODO use static lock for glfw usage!!
  m_resultDrawFrame = vkAcquireNextImageKHR(
      *mp_device,
      *mp_swapChain,
      std::numeric_limits<uint64_t>::max(),
      m_imageAvailableSemaphores[m_currentFrame],
      VK_NULL_HANDLE,
      &m_nextImgIndex);
  if(m_nextImgIndex>m_vecImageInFlight.size()){//sometimes m_nextImgIndex is strange...
    mp_context->printErr("MFVkRenderLoop::drawFrame - vkAcquireNextImageKHR returned "
        "wrong index!");
    return;
  }

  if(m_vecImageInFlight[m_nextImgIndex]!=VK_NULL_HANDLE){
    vkWaitForFences(//wait for the fence, if available, the frame isnt used.
        *mp_device,
        1,
        &m_vecImageInFlight[m_nextImgIndex],
        VK_TRUE,
        std::numeric_limits<uint64_t>::max());
  }
  m_vecImageInFlight[m_nextImgIndex]=m_inFlightFences[m_currentFrame];

  if(m_resultDrawFrame==VK_ERROR_OUT_OF_DATE_KHR){
    mp_resources->recreateSwapChain();
    reinitRenderLoop();
    mp_context->getWindowContainer()->pollWindowResize();
    mp_context->printInfo("vkAcquireNextImageKHR() VK_ERROR_OUT_OF_DATE_KHR");
    return;
  }else if (m_resultDrawFrame != VK_SUCCESS && m_resultDrawFrame != VK_SUBOPTIMAL_KHR){//&& m_resultDrawFrame != VK_SUBOPTIMAL_KHR
    mp_context->printErr("vkAcquireNextImageKHR() VK_SUBOPTIMAL_KHR");
    throw std::runtime_error("MFVkRenderLoop::drawFrame() - failed!");
  }

  m_submitInfo.pWaitSemaphores = &(m_imageAvailableSemaphores[m_currentFrame]);
  m_submitInfo.pCommandBuffers = &mp_vecCommandBuffer->at(m_nextImgIndex);
  m_submitInfo.pSignalSemaphores = &(m_renderFinishedSemaphores[m_currentFrame]);

  vkResetFences(*mp_device, 1, &m_inFlightFences[m_currentFrame]);

  if (vkQueueSubmit(
      *mp_context->getDevice()->getGraphicsQueue(),
      1,
      &m_submitInfo,
      m_inFlightFences[m_currentFrame]) != VK_SUCCESS) {
    mp_context->printErr("MFVkRenderLoop::drawFrame - cant submit data to graphics queue!");
    return;
  }
  m_presentInfo.pWaitSemaphores = &(m_renderFinishedSemaphores[m_currentFrame]);;
  m_presentInfo.pImageIndices = &m_nextImgIndex;//TODO this data changes!

  //	VkSwapchainKHR swapChains[] = {*};

  m_resultDrawFrame = vkQueuePresentKHR(
      *mp_context->getDevice()->getPresentationQueue(),
      &m_presentInfo);

  /*recreate if sth changed or throw failure if m_resultDrawFrame is not successful*/
  if (m_resultDrawFrame == VK_ERROR_OUT_OF_DATE_KHR || m_resultDrawFrame == VK_SUBOPTIMAL_KHR ) {//|| framebufferResized
    mp_resources->recreateSwapChain();
    mp_context->getWindowContainer()->pollWindowResize();
    reinitRenderLoop();
    mp_context->printWarning("MFVkRenderLoop::drawFrame() - "
        "recreate swap chain!");
  } else if (m_resultDrawFrame != VK_SUCCESS) {
    mp_context->printErr("MFVkRenderLoop::drawFrame() - "
        "failed to present swap chain image!");
  }
  lockUpdateTrigger.lock();
  if(m_updateCommandBuffer){
    m_updateCommandBuffer=false;
    lockUpdateTrigger.unlock();
    vkQueueWaitIdle(*mp_context->getDevice()->getGraphicsQueue());
    updateCommandBuffer();
  }else{
    lockUpdateTrigger.unlock();
  }
  m_currentFrame=(m_currentFrame+1)%m_framesInFlight;
}

bool MFVkRenderLoop::updateCommandBuffer(){
  bool ret=true;

  for(uint32_t i=0;i<mp_vecCommandBuffer->size();i++){
    ret&=setupCommandBuffer(
        &mp_vecCommandBuffer->at(i),
        mp_vecFramebuffer->at(i),i);
  }
  return ret;
}

void MFVkRenderLoop::triggerCommandBufferUpdate(){
  lockUpdateTrigger.lock();
  m_updateCommandBuffer=true;
  lockUpdateTrigger.unlock();
}

bool MFVkRenderLoop::setupCommandBuffer(
    VkCommandBuffer* pBuffer,
    VkFramebuffer* pFramebuffer,
    uint32_t index){
  mp_renderPassInfo->sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  mp_renderPassInfo->renderPass = *mp_renderPass;
  mp_renderPassInfo->renderArea.offset = {0, 0};
  mp_renderPassInfo->renderArea.extent.width =
      mp_resources->getSwapchain()->getExtent().width;
  mp_renderPassInfo->renderArea.extent.height =
      mp_resources->getSwapchain()->getExtent().height;
  mp_renderPassInfo->clearValueCount = static_cast<uint32_t>(
      mp_clearValues->size());
  mp_renderPassInfo->pClearValues = mp_clearValues->data();
  mp_renderPassInfo->framebuffer = *pFramebuffer;
  //record command buffer
  VkCommandBufferBeginInfo beginInfo = {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
  if (vkBeginCommandBuffer(*pBuffer, &beginInfo) != VK_SUCCESS) {
    mp_context->printErr("Failed to setup command buffer!");
    return false;
  }
  //depending on objects the render pass and pipeline may change!
  //TODO update render pass if recreate... is called!
  vkCmdBeginRenderPass(*pBuffer, mp_renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  for(MFAbstractSetup* pSetup:*mp_vecObjectSetups){
    pSetup->setupRenderCommands(pBuffer);//sets up all cmd buffers for added objects
  }

  vkCmdEndRenderPass(*pBuffer);
  //End command buffer
  if (vkEndCommandBuffer(*pBuffer) != VK_SUCCESS) {
    mp_context->printErr("setup commandbuffer failed!");
    return false;
  }
  return true;
}

void printBufferInfo(){
  std::string txt="Object infos:\n";
  //	VkMemoryRequirements* memReq=new VkMemoryRequirements();
  //	vkGetBufferMemoryRequirements(*mp_context->getVirtualDevice(),
  //					*pObject->getVertexBuffer(),
  //					memReq);
  //	txt+="vtx_buf_size:"+std::to_string(memReq->size);
  //	vkGetBufferMemoryRequirements(*mp_context->getVirtualDevice(),
  //					*pObject->getIndexBuffer(),
  //					memReq);
  //	txt+=" / idx_buf_size:"+std::to_string(memReq->size);
  //	txt+=" / n_ver: "+std::to_string(pObject->getVerticesCount());
  //	txt+=" / n_ind: "+std::to_string(pObject->getIndicesCount())+"\n";
}

void MFVkRenderLoop::setFramesInFlight(uint32_t count){
  m_framesInFlight=count;
}

