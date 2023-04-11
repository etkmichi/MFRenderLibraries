/*
 * MFRenderThread.h
 *
 *  Created on: 15.07.2019
 *      Author: michl
 */

#ifndef MFRENDERCLASSES_MFRENDERTHREAD_H_
#define MFRENDERCLASSES_MFRENDERTHREAD_H_
#include <MFThreadSystem/MFTaskThread.h>

class MFRenderThread : public MFTaskThread {
public:
	MFRenderThread();
	virtual ~MFRenderThread();
};


#endif /* MFRENDERCLASSES_MFRENDERTHREAD_H_ */
