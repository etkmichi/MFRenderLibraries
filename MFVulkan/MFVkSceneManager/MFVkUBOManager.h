/*
 * MFUBOManager.h
 *
 *  Created on: 28.08.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKSCENEMANAGER_MFVKUBOMANAGER_H_
#define MFVULKAN_MFVKSCENEMANAGER_MFVKUBOMANAGER_H_

class MFVkUBOManager;
#include <MFData/MFDataObject.h>
#include "../MFVkObjects/MFVkBuffer.h"
#include <vulkan/vulkan.h>
#include <MFObjects/MFObject.h>
#include "../MFVkObjects/MFVkStructs.h"
#include "../MFVkContext/MFVkContext.h"
#include "../MFVkResources/MFVkResources.h"
#include "../../MFRenderContext.h"
//TODO create class with predefined pointers to uniform buffers like transformation
//color, array,... add an object of this class to S_UniformBuffer and setup this object
//by a virtual class in MFAbstractSetup -> the object will provide ptr access to the
//stored buffers in S_UniformBuffer::pVecDataBuffers. With such an implementation, the
//devoloper does not need to know the position of specific data within
//S_UniformBuffer::pVecDataBuffers.
/**
 * This class provides functions for uniform data setup. With the add... functions
 * data can be structured. After setting up an object of this class with the add...
 * functions, the initUBOManager function can be called to create all uniform buffers.
 * During usage in a scene, new elements can get a new ubo buffer by calling
 * getNextUBOBuffer(). The returned structure (S_UniformBuffer) of getNextUBOBuffer()
 * contains a vector filled with MFVkBuffer* (pVecDataBuffers). Each element in
 * pVecDataBuffers represents a uniform value (f.e. color, transformation matrice,
 * an index, ...). The MFVkBuffer class provides functions to overwrite the uniform data.
 */
class MFVkUBOManager {

private:
  std::vector<S_UniformBuffer*>
  *mp_vecUniformBuffers;

  std::vector<MFDataObject>
  *mp_vecUBOSetup;

  std::vector<void*>
  *mp_vecInitialDataDummies;

  std::vector<void*>
  *mp_vecUBOMemory;

  std::vector<MFVkBuffer*>//Contains all created buffers
  *mp_vecVkBuffers;//just for tracking and recreating

  uint32_t
  m_maxObjectCount=1,
  m_nextBufferIndex=0;

  MFRenderContext
  *mp_context;

public:
  MFVkUBOManager(MFRenderContext* pContext);

  virtual ~MFVkUBOManager();
  void destroySubBuffers();
  void clearAllocatedUBOBuffer();


  bool recreateBuffers();

  /**
   * Allocates memory on the host for the uniform data. Uses the added data type setup
   * for allocating. Allocated data:
   * 	if MFDataObject->isStatic()==false ->maxObjectCount*(MFDataObject->getByteSize())
   * 	if MFDataObject->isStatic()==true ->1*(MFDataObject->getByteSize())
   *
   * @return
   */
  bool allocateHostUniformMemory();

  /** TODO explain reused buffers
   * This method will allocate the memory for the ubo buffers. This function can be
   * used for recreation. It allocates memory for the vulkan buffer and local host
   * memory. It uses the host memory as data for the vulkan buffer. The first init
   * call will use the initial data setup of added data type, all further calls to
   * this init function will copy the already allocate host memory to the vulkan buffer.
   * If initial data must be used, call clearAllocatedUBOBuffer before using this func..
   * @return
   */
  bool initUboManager(MFVkContext* pContext, MFVkResources* pResources);
  void setMaxUBOCount(uint32_t count);

  S_UniformBuffer* getUBOBuffer(uint32_t index);

  /**
   *
   * @return
   */
  S_UniformBuffer* getNextUBOBuffer();

  uint32_t getMaxObjectCount(){return m_maxObjectCount;};
  std::vector<S_UniformBuffer*>* getVecBuffers(){return mp_vecUniformBuffers;};
  void addDataType(MFDataObject* pDataObject);
  void addDataType4x4(glm::mat4x4* initialData);
  void addDataTypeUINT32(uint32_t* initialData);
  //TODO std::vector<MFVkBuffer*>* getNextUBOObject()
  //TODO reinit -> destroy all used resources set to initial
  //TODO destroyResources

};

#endif /* MFVULKAN_MFVKSCENEMANAGER_MFVKUBOMANAGER_H_ */
