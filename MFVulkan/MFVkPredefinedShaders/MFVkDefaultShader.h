/*
 * MFVkDefaultShader.h
 *
 *  Created on: 19.09.2019
 *      Author: michl
 */

#ifndef MFVULKAN_MFVKPREDEFINEDSHADERS_MFVKDEFAULTSHADER_H_
#define MFVULKAN_MFVKPREDEFINEDSHADERS_MFVKDEFAULTSHADER_H_

class MFVkDefaultShader;

#include <iostream>
#include <fstream>
#include "MFObjects/MFObject.h"
#include "vulkan/vulkan.h"
#include "../MFVkObjects/MFVkShader.h"
#include "../MFVkContext/MFVkContext.h"
#include "../MFVkObjectComponents/MFVkPipeline.h"
//#define BOOST_NO_CXX17_SCOPED_ENUMS
//#undef BOOST_NO_CXX17_SCOPED_ENUMS
#define COMPILE_SHADER

class MFVkDefaultShader {

private:
	MFVkContext*
		mp_context;

	MFVkShader
		*mp_defaultVertexShader,
		*mp_defaultFragmentShader;

	bool
		m_loadFromFile=true;

public:
	MFVkDefaultShader(MFVkContext* pContext);
	virtual ~MFVkDefaultShader();
	void writeShaderToFile(bool writeEnable){m_loadFromFile=!writeEnable;};

	/**
	 * Path variables for glsl shader compile must be set and a valid applicaiton path must be available!
	 * @return
	 */
	bool initDefaultShader();

	/**
	 * Init for external shaders.
	 * @param pVertexShader
	 * @param pFragmentShader
	 * @return
	 */
	bool initDefaultShader(MFVkShader* pVertexShader,MFVkShader* pFragmentShader);
	bool initDefaultVertexShader(MFVkShader* pVertexShader);
	bool initDefaultFragmentShader(MFVkShader* pFragmentShader);
	MFVkShader* getFragmentShader();
	MFVkShader* getVertexShader();
	bool writeShaderToDisk();

	const char* vertexShader="\
#version 450\n\
#extension GL_ARB_separate_shader_objects : enable\n\
\n\
layout(binding = 0) uniform UBOModel{\n\
	mat4 model;\n\
} uboModel;\n\
layout(binding = 1) uniform UBOView{\n\
	mat4 view;\n\
} uboView;\n\
layout(binding = 2) uniform UBOProjection{\n\
	mat4 projection;\n\
} uboProjection;\n\
\n\
layout(location = 0) in vec3 inPosition;\n\
layout(location = 0) out vec3 fragColor;\n\
\n\
void main() {\n\
	//gl_Position = uboProjection.projection * uboView.view * uboModel.model *vec4(inPosition, 1.0);\n\
	//gl_Position = vec4(inPosition, 1.0);\n\
	int j=0;\n\
	vec4 position=uboModel.model *vec4(inPosition, 1.0);\n\
	if(position.x > 2.1 || position.x <-2.1){\n\
		while (j<2000000){\n\
			j+=1;\n\
			gl_Position = uboProjection.projection * uboView.view * uboModel.model *vec4(inPosition, 1.0);\n\
		}\n\
	}\n\
	gl_Position = uboProjection.projection * uboView.view * uboModel.model *vec4(inPosition, 1.0);\n\
}";
	/*
	 	vec2 positions[3] = vec2[](\n\
		    vec2(0.0, -10.5),\n\
		    vec2(10.5, 10.5),\n\
		    vec2(-10.5, 0.5)\n\
		);		\n\
		vec3 colors[3] = vec3[](\n\
		    vec3(1.0, 0.0, 0.0),\n\
		    vec3(0.0, 1.0, 0.0),\n\
		    vec3(0.0, 0.0, 1.0)\n\
		);		\n\
	uboProjection.projection * uboView.view * uboModel.model
	*/

const char* fragmentShader="\
#version 450\n\
#extension GL_ARB_separate_shader_objects : enable\n\
\n\
layout(binding = 3)uniform UBOColor{\n\
	vec4 color;\n\
} uboColor;\n\
layout(location = 0) in vec3 fragColor;\n\
\n\
layout(location = 0) out vec4 outColor;\n\
\n\
void main() {\n\
	outColor = vec4(0.5,0.5,0.5, 0.5);\n\
}";
};

#endif /* MFVULKAN_MFVKPREDEFINEDSHADERS_MFVKDEFAULTSHADER_H_ */
