/*
 * MFVkSceneObjectBuffers.h
 *
 *  Created on: 17.01.2020
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKOBJECTS_MFVKSCENEOBJECTBUFFERS_H_
#define MFVULKAN_MFVKOBJECTS_MFVKSCENEOBJECTBUFFERS_H_

class MFVkSceneObjectBuffers;

#include "../MFVkObjects/MFVkBuffer.h"
/**
 * This class provides predefined buffer pointer. The predefined buffers are used
 * to keep track of buffers, even if the user doesn't know where the buffers are
 * stored. This may come in handy if a lot of implementations of f.e. MFAbstractSetup
 * exist.
 * Every pointer can be set to point to a specific buffer. The class provides setter and getter functions.
 * The buffers are named in a understandable way.
 *
 * TODO explain all pointers.
 */
class MFVkSceneObjectBuffers {
private:
	static MFVkBuffer
		*smp_dummyBuffer;

	MFVkBuffer
		*mvpMatrices=nullptr,
		*color=nullptr,
		*modelMatrix=nullptr,
		*viewMatrix=nullptr,
		*perspectiveMatrix=nullptr,
		*vertexIndices=nullptr,
		*vertices=nullptr;
	//TODO buffer setup for each buffer -> provides write/transformation conditions
	//for valid usage f.e. color could be a uint32_t or a glm::vec3
	//TODO overloaded update functions which uses the buffer setup for valid updates
	//f.e. updateColor(uint32_t)->updates the color independent by the color data type
	//which is used.
	//TODO update data library which is specified for independ data usage
public://TODO is a dummy buffer better as nullptr?
	MFVkSceneObjectBuffers();
	virtual ~MFVkSceneObjectBuffers();

	/**
	 * All buffers will be set to point to a static dummy value. This will prevent
	 * nullptr access!
	 */
	void initDummySetup();
	void setMVPMatrices(MFVkBuffer* pBuffer){mvpMatrices=pBuffer;};
	void setColor(MFVkBuffer* pBuffer){color=pBuffer;};
	//void set___(MFVkBuffer* pBuffer){=pBuffer;};
	MFVkBuffer* getColor(){return color;};
	MFVkBuffer* getMVPMatrices() {
		return mvpMatrices;
	}
	MFVkBuffer*& getModelMatrix() {
		return modelMatrix;
	}

	void setModelMatrix(MFVkBuffer *modelMatrix = nullptr) {
		this->modelMatrix = modelMatrix;
	}

	MFVkBuffer*& getPerspectiveMatrix() {
		return perspectiveMatrix;
	}

	void setPerspectiveMatrix(MFVkBuffer *perspectiveMatrix = nullptr) {
		this->perspectiveMatrix = perspectiveMatrix;
	}

	MFVkBuffer*& getViewMatrix() {
		return viewMatrix;
	}

	void setViewMatrix(MFVkBuffer *viewMatrix = nullptr) {
		this->viewMatrix = viewMatrix;
	}
	//MFVkBuffer* get___(){return ;};
};

#endif /* MFVULKAN_MFVKOBJECTS_MFVKSCENEOBJECTBUFFERS_H_ */
