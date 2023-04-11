/*
 * MFVkSceneObjectBuffers.cpp
 *
 *  Created on: 17.01.2020
 *      Author: michl
 */

#include "MFVkSceneObjectBuffers.h"
MFVkBuffer* MFVkSceneObjectBuffers::smp_dummyBuffer=new MFVkBuffer(nullptr, nullptr, 256);
MFVkSceneObjectBuffers::MFVkSceneObjectBuffers() {
	smp_dummyBuffer->invalidate();
}

MFVkSceneObjectBuffers::~MFVkSceneObjectBuffers() {
	delete smp_dummyBuffer;
}

void MFVkSceneObjectBuffers::initDummySetup(){
	mvpMatrices=smp_dummyBuffer;
	color=smp_dummyBuffer;
	modelMatrix=smp_dummyBuffer;
	viewMatrix=smp_dummyBuffer;
	perspectiveMatrix=smp_dummyBuffer;
	vertexIndices=smp_dummyBuffer;
	vertices=smp_dummyBuffer;
}
