/*
 * MFWindowContainer.cpp
 *
 *  Created on: 15.07.2019
 *      Author: michl
 */

#include "MFWindowContainer.h"

MFWindowContainer::MFWindowContainer(std::string title,MFRenderContext* pContext):
m_title(title),
m_isResized(false),
m_isClosed(false),
lockResources(){
  mp_context=pContext;
  if(mp_context==nullptr){
    mp_context=MFRenderContext::smp_staticContext;
  }
	mp_cbResizeFramebuffer=nullptr;
	mp_cbWindowClose=nullptr;
	mp_window=nullptr;
	m_width=256;
	m_height=256;
	funcPointerResize=nullptr;
}

MFWindowContainer::~MFWindowContainer() {
	lockResources.lock();
	glfwDestroyWindow(mp_window);
	glfwTerminate();
	lockResources.unlock();
}

void MFWindowContainer::glfwErrorCallback(int errCode,const char* data){
  MFRenderContext::smp_staticContext->printErr(
      "MFWindowContainer::glfwErrorCallback - (code "+std::to_string(errCode)+") "+
      std::string(data));
}

bool MFWindowContainer::initWindow(){
	lockResources.lock();
	mp_context->printInfo("MFWindowContainer::initWindow() - Init glfw");
	if(GLFW_FALSE == glfwInit()){
		mp_context->printErr("Couldn't initialize glfw!");
		return false;
	}
	mp_context->printInfo("MFWindowContainer::initWindow() - Setting GLFW Error callback");
	glfwSetErrorCallback(MFWindowContainer::glfwErrorCallback);


	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	mp_window = glfwCreateWindow(m_width, m_height, m_title.data(), NULL, NULL);
	if(mp_window!=nullptr){
		glfwSetWindowUserPointer(mp_window, this);
		glfwSetFramebufferSizeCallback(mp_window,&MFWindowContainer::frambufferResizeCallback);
		glfwSetWindowCloseCallback(mp_window, &(MFWindowContainer::windowCloseCallback));
		mp_context->printInfo("MFWindowContainer::initWindow() - glfw window initialized!");
		lockResources.unlock();
	}else{
		mp_context->printErr("window wasn't initialized!!");
		lockResources.unlock();
		return false;
	}
	return true;
}

/*static*/
void MFWindowContainer::frambufferResizeCallback(
		GLFWwindow* window,
		int width,
		int height){
	MFRenderContext::smp_staticContext->printInfo(""
	    "MFWindowContainer::frambufferResizeCallback - width/height"+
			std::to_string(width)+"/"+std::to_string(height));
	MFWindowContainer *container=reinterpret_cast<MFWindowContainer*>(glfwGetWindowUserPointer(window));
	if(container!=nullptr)
		container->frambufferResize(width,height);
}

void MFWindowContainer::frambufferResize(int width, int height){
	lockResources.lock();
	m_height=height;
	m_width=width;
	m_isResized=true;

	if(mp_cbResizeFramebuffer!=nullptr){
		mp_cbResizeFramebuffer(width,height);
	}
	lockResources.unlock();
}

/*static*/
void MFWindowContainer::windowCloseCallback(GLFWwindow* window){
	MFWindowContainer *container=
			reinterpret_cast<MFWindowContainer*>(glfwGetWindowUserPointer(window));
	MFRenderContext::smp_staticContext->printInfo(
	    "MFWindowContainer - window close pressed!");
	if(container!=nullptr)
		container->windowClose(window);
}

void MFWindowContainer::windowClose(GLFWwindow* window){
	lockResources.lock();
	m_isClosed=true;
	if(mp_cbWindowClose!=nullptr){
		mp_cbWindowClose(window);
	}
	lockResources.unlock();
}
bool MFWindowContainer::pollWindowClose(){
	lockResources.lock();
	bool ret=m_isClosed;
	m_isClosed=false;
	lockResources.unlock();
	return ret;
}
bool MFWindowContainer::pollWindowResize(){
	lockResources.lock();
	bool ret=m_isResized;
	m_isResized=false;
	lockResources.unlock();
	return ret;
}

bool MFWindowContainer::isClosed(){
	glfwPollEvents();
	m_isClosed=glfwWindowShouldClose(mp_window);
	return m_isClosed;
}

/**
 *
 * @param cbFunction callback(windowWidth,windowHeight
 */
void MFWindowContainer::setFrambufferResizeCallback(resizeCallbackFunction resizeCallback){
	/*test*/
	lockResources.lock();
	mp_cbResizeFramebuffer=resizeCallback;
	lockResources.unlock();
}

void MFWindowContainer::setWindowCloseCallback(closeCallbackFunction closeCallback){
	/*test*/
	lockResources.lock();
	mp_cbWindowClose=closeCallback;
	lockResources.unlock();
}

