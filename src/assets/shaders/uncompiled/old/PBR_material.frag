#version 450

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec3 inFragPos;
layout(location = 2) in vec3 inNormal;

layout(set = 0, binding = 1) uniform lightBuf {
	vec4 ambiantColor;
	vec4 color;
	vec3 position;
	vec3 viewPosition;
} lights;

layout(set = 1, binding = 0) uniform sampler2D colorTexture;
layout(set = 1, binding = 1) uniform sampler2D normalTexture;
layout(set = 1, binding = 2) uniform sampler2D roughnessTexture;
layout(set = 1, binding = 3) uniform sampler2D metalnessTexture;
layout(set = 1, binding = 4) uniform sampler2D heightTexture;
layout(set = 1, binding = 5) uniform sampler2D aoTexture;

layout (location = 0) out vec4 outFragColor;

void main() 
{
	vec3 ambiant = lights.ambiantColor.w * lights.ambiantColor.xyz;

	float dist = length(lights.viewPosition - inFragPos);

	vec3 norm = normalize(inNormal);
	vec3 lightDir = normalize(lights.position - inFragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lights.color.xyz / dist * lights.color.w;

	float spec = 0.0;
	// if (diff > 0) {
		vec3 viewDir = normalize(lights.viewPosition - inFragPos);
		vec3 halfwayDir = normalize(lightDir + viewDir);
		spec = pow(max(dot(norm, halfwayDir), 0.0), 32);
	// }
	vec3 specular = spec * lights.color.xyz / dist * lights.color.w;
	
	outFragColor = texture(colorTexture, inUV) * vec4(ambiant + specular, 1.0f);
}