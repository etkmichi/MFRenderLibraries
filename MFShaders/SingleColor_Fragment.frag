#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1)uniform UBOColor{
	vec4 color;
} uboColor;

layout (location = 0) in vec3 normal;
layout (location = 0) out vec4 outColor;

void main() {
	outColor.x = uboColor.color[0]*normal.x;
	outColor.y = uboColor.color[1]*normal.y;
	outColor.z = uboColor.color[2]*normal.z;
	outColor.w = uboColor.color[3];
}

