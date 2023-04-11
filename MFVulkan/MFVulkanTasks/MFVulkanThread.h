/*
 * MFVulkanThread.h
 *
 *  Created on: 15.07.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVULKANTHREAD_H_
#define MFVULKAN_MFVULKANTHREAD_H_
#include "../../MFRenderClasses/MFRenderThread.h"

class MFVulkanThread: public MFRenderThread {
public:
	MFVulkanThread();
	virtual ~MFVulkanThread();
};


#endif /* MFVULKAN_MFVULKANTHREAD_H_ */
