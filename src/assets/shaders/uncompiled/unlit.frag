#version 450

layout(location = 0) in vec2 inUV;

layout (location = 0) out vec4 outFragColor;

// material descriptor set
layout(set = 1, binding = 0) uniform sampler2D albedoMap;

// main shader
void main() 
{	
	outFragColor = texture(albedoMap, inUV);
}