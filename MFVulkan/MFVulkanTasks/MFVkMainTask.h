/*
 * MFVkMainTask.h
 *
 *  Created on: 15.07.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVULKANTASKS_MFVKMAINTASK_H_
#define MFVULKAN_MFVULKANTASKS_MFVKMAINTASK_H_

#include <MFTasks/MFAbstractTask.h>

class MFVkMainTask: public MFAbstractTask {
public:
	MFVkMainTask();
	virtual ~MFVkMainTask();
	bool doWork();
	bool undoWork();
};

#endif /* MFVULKAN_MFVULKANTASKS_MFVKMAINTASK_H_ */
