#version 450

layout (location = 0) in vec4 inColor;
layout(location = 1) in vec3 inFragPos;
layout(location = 2) in vec3 inNormal;

layout (location = 0) out vec4 outFragColor;

layout(set = 0, binding = 1) uniform lightBuf {
	vec4 ambiantColor;
	vec4 diffuseColor;
	vec3 diffusePosition;
} lights;

void main() 
{
	vec3 norm = normalize(inNormal);
	vec3 lightDir = normalize(lights.diffusePosition - inFragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lights.diffuseColor.xyz;

	vec3 ambiant = lights.ambiantColor.xyz * lights.ambiantColor.w;

	outFragColor = inColor * vec4(ambiant + diffuse, 1.0f);
}