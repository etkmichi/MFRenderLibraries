/*
 * MFWindowContainer.h
 *
 *  Created on: 15.07.2019
 *      Author: michl
 */

#ifndef MFWINDOW_MFWINDOWCONTAINER_H_
#define MFWINDOW_MFWINDOWCONTAINER_H_
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <mutex>
#include "MFObjects/MFObject.h"
#include "vulkan/vulkan.h"
#include "../MFRenderContext.h"
/**
 * This class wrapps around a glfw window.
 * TODO test if window settings can be changed on a separated thread and without synchronisation.
 */
class MFWindowContainer {
public:
	/**
	 *
	 * @param window to resize
	 * @param width
	 * @param height
	 */
	typedef void (* resizeCallbackFunction)(int,int);

	/**
	 *
	 * @param window to close
	 */
	typedef void (* closeCallbackFunction)(GLFWwindow*);

private:
	std::string
		m_title;

	bool
		m_isResized,
		m_isClosed;

	/*external resize callback function*/
	/**
	 *
	 * @param int width of window
	 * @param int height of window
	 */
	void(*mp_cbResizeFramebuffer)(int,int);
	closeCallbackFunction mp_cbWindowClose;

	std::mutex
		lockResources,
		lockCallback;

	void (MFWindowContainer::*funcPointerResize)(GLFWwindow*,int,int);

	GLFWwindow*
		mp_window;

	MFRenderContext
	*mp_context;

	unsigned int
		m_width,m_height;
public:
	MFWindowContainer(std::string title,MFRenderContext* pContext);
	virtual ~MFWindowContainer();
	bool initWindow();
	static void glfwErrorCallback(int errCode,const char* data);

	/**
	 * Sets a callback function which will be called after resizing a window.
	 * The width and height fields will be updated before cbFunction call.
	 * @param cbFunction callback function.
	 */
	void setFrambufferResizeCallback(resizeCallbackFunction resizeCallback);
	void setWindowCloseCallback(closeCallbackFunction closeCallback);

	/**
	 * Returns ture if the window was resized. The resize flag will be set to false. Next call to this function will return false.
	 * @return Returns if the window was resized.
	 */
	bool pollWindowResize();
	bool pollWindowClose();
	bool isClosed();
	void setWidth(unsigned int width){lockResources.lock();m_width=width;lockResources.unlock();};
	void setHeight(unsigned int height){lockResources.lock();m_height=height;lockResources.unlock();};
	/*TODO implement add window for multiple windows.*/

	/**
	 * Used as callback function for glfw. The userpointer of the window will be used to call this->frambufferResize(...) function.
	 * @param window - GLFWwindow pointer with MFWindowContainer object as user pointer.
	 * @param width
	 * @param height
	 */
	static void frambufferResizeCallback(GLFWwindow* window,int width,int height);

	void frambufferResize(int width,int height);

	static void windowCloseCallback(GLFWwindow* window);

	void windowClose(GLFWwindow* window);

	GLFWwindow* getWindow(){return mp_window;};
};

#endif /* MFWINDOW_MFWINDOWCONTAINER_H_ */
