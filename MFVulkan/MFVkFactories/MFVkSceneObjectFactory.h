/*
 * MFVkSceneObjectFactory.h
 *
 *  Created on: 17.09.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKFACTORIES_MFVKSCENEOBJECTFACTORY_H_
#define MFVULKAN_MFVKFACTORIES_MFVKSCENEOBJECTFACTORY_H_

#include "../MFVkObjects/MFVkShader.h"
#include "../MFVkObjectComponents/MFVkPipeline.h"
#include "../MFVkResources/MFVkResources.h"
#include "../MFVkContext/MFVkContext.h"
/**
 * This class can be used for creating specific scene objects with the neccessary resources.
 */
class MFVkSceneObjectFactory {
	MFVkContext
		*mp_context;
	MFVkResources
		*mp_resources;

	/*The shader codes are located in the MFShaders folder within the MFRenderLibraries project*/
	MFVkShader
		*mp_shader_Fragment_SingleColor,
		*mp_shader_Vertex_SingleColor,
		*mp_shader_Fragment_TexturedModel,
		*mp_shader_Vertex_TexturedModel;

	MFVkPipeline
		*mp_pipeSingleColor,
		*mp_pipeModel;
public:
	MFVkSceneObjectFactory(MFVkContext *pContext, MFVkResources *pResources);
	bool initFactory();
	virtual ~MFVkSceneObjectFactory();
//	MFVkInterfaceSceneObject* create3DModel(std::string geometryPath,std::string texturePath);
//	MFVkInterfaceSceneObject* create2DTextureSurface(std::string texturePath);
//	MFVkInterfaceSceneObject* create3DSphere(uint32_t rgbaHexColor);
//
//	bool setup3DModel(MFVkInterfaceSceneObject& sceneObject,std::string geometryPath,std::string texturePath);
//	bool setup3DSphere(MFVkInterfaceSceneObject& sceneObject,uint32_t rgbaHexColor);
//	bool setup2DTextureSurface(MFVkInterfaceSceneObject& sceneObject,std::string texturePath);
};

#endif /* MFVULKAN_MFVKFACTORIES_MFVKSCENEOBJECTFACTORY_H_ */
