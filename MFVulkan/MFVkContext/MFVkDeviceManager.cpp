/*
 * MFVkDeviceManager.cpp
 *
 *  Created on: 05.08.2019
 *      Author: michl
 */

#include "MFVkDeviceManager.h"

MFVkDeviceManager::MFVkDeviceManager(MFRenderContext* pContext) :
m_deviceCount(0)
{
  mp_context=pContext;
  if(mp_context==nullptr){
    mp_context=MFRenderContext::smp_staticContext;
  }
  mp_instance=nullptr;
  mp_vecPhysicalDevices=nullptr;
  mp_vecPhysicalDeviceProperties=nullptr;
}

MFVkDeviceManager::MFVkDeviceManager(
    VkInstance* pInstance,
    MFRenderContext* pContext):
m_deviceCount(0)
{
  mp_context=pContext;
  if(mp_context==nullptr){
    mp_context=MFRenderContext::smp_staticContext;
  }
  mp_instance=pInstance;
  mp_vecPhysicalDevices=nullptr;
  mp_vecPhysicalDeviceProperties=nullptr;
  initDeviceManager(pInstance);
}

MFVkDeviceManager::~MFVkDeviceManager() {
  // TODO Auto-generated destructor stub
}

bool MFVkDeviceManager::initDeviceManager(VkInstance* pInstance){
  /*Retrieve physical device handles*/
  mp_context->printInfo("MFVkDeviceManager::initDeviceManager");
  if(pInstance==nullptr){
    printErr("MFVkDeviceManager::initDeviceManager - VkInstance must not be nullptr!");
    return false;
  }
  mp_instance=pInstance;
  vkEnumeratePhysicalDevices(*mp_instance, &m_deviceCount, nullptr);
  if (m_deviceCount == 0) {
    printErr("no vulkan devices found!!!");
    return false;
  }
  if(mp_vecPhysicalDevices!=nullptr){
    mp_vecPhysicalDevices->clear();
    delete mp_vecPhysicalDevices;
  }
  mp_vecPhysicalDevices=new std::vector<VkPhysicalDevice>(m_deviceCount);
  VkResult res=vkEnumeratePhysicalDevices(*mp_instance, &m_deviceCount, mp_vecPhysicalDevices->data());

  if(res!=VK_SUCCESS){
    mp_context->printErr("MFVkDeviceManager::initDeviceManager - "
        "failed to enumerate devices! VkResult="+std::to_string(res));
  }
  /*retrieve physical device properties*/
  if(mp_vecPhysicalDeviceProperties!=nullptr){
    mp_vecPhysicalDeviceProperties->clear();
    delete mp_vecPhysicalDeviceProperties;
  }
  mp_vecPhysicalDeviceProperties = new std::vector<VkPhysicalDeviceProperties>(m_deviceCount);

  for(unsigned int i=0;i<m_deviceCount;i++){
    vkGetPhysicalDeviceProperties(mp_vecPhysicalDevices->at(i), &mp_vecPhysicalDeviceProperties->at(i));
  }
  return true;
}

std::vector<VkPhysicalDevice>* MFVkDeviceManager::getPhysicalDevices(){
  return mp_vecPhysicalDevices;
}

void MFVkDeviceManager::listDevices(){
  if(mp_vecPhysicalDevices==nullptr){
    printErr("device manage not initialiced!");
  }
  std::string info="Vulkan physical devices (device count "+std::to_string(m_deviceCount)+"):\n";
  int counter=0;
  for(VkPhysicalDeviceProperties deviceProperties:(*mp_vecPhysicalDeviceProperties)){
    info+="device number: "+std::to_string(counter)+" - device name: "+std::string(deviceProperties.deviceName)+"\n";
  }
  printInfo(info);
}

