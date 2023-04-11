/*
 * MFVkTaskInit.h
 *
 *  Created on: 15.07.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVULKANTASKS_MFVKTASKINIT_H_
#define MFVULKAN_MFVULKANTASKS_MFVKTASKINIT_H_

#include <MFTasks/MFAbstractTask.h>
/**
 * This class initializes the vulkan context.
 */
class MFVkTaskInit: public MFAbstractTask {
public:
	MFVkTaskInit();
	virtual ~MFVkTaskInit();
	bool doWork();
	bool undoWork();
};

#endif /* MFVULKAN_MFVULKANTASKS_MFVKTASKINIT_H_ */
