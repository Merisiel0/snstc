#version 450

layout(location = 0) in vec3 texCoords;

layout(location = 0) out vec4 fragColor;

// material descriptor set
layout(set = 1, binding = 0) uniform samplerCube skybox;

// main shader
void main()
{
  fragColor = texture(skybox, texCoords);
}