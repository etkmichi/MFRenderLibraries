/*
 * MFVkSceneObjectFactory.cpp
 *
 *  Created on: 17.09.2019
 *      Author: michl
 */

#include "MFVkSceneObjectFactory.h"

MFVkSceneObjectFactory::MFVkSceneObjectFactory(MFVkContext *pContext, MFVkResources *pResources) {
	mp_context=pContext;
	mp_resources=pResources;
	mp_shader_Fragment_SingleColor=nullptr;
	mp_shader_Vertex_SingleColor=nullptr;
	mp_shader_Fragment_TexturedModel=nullptr;
	mp_shader_Vertex_TexturedModel=nullptr;
	mp_pipeSingleColor=nullptr;
	mp_pipeModel=nullptr;
	// TODO Auto-generated constructor stub
}

MFVkSceneObjectFactory::~MFVkSceneObjectFactory() {
	// TODO Auto-generated destructor stub
}

