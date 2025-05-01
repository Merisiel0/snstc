#version 450
#extension GL_EXT_buffer_reference : require

layout(location = 0) out vec3 texCoords;

// global descriptor set
layout(std140, set = 0, binding = 0) uniform GlobalBuffer {
  mat4 projection;
  mat4 view;
} global;

// push constants (replacing object descriptor set)
struct Vertex {
	vec3 position;
	float u; 
	vec3 normal;
	float v;
}; 

layout(buffer_reference, std430) readonly buffer VertexBuffer {
	Vertex vertices[];
};

layout( push_constant ) uniform PushConstants {
  mat4 model;
	VertexBuffer vertBuf;
} pushConstants;

void main()
{
  Vertex v = pushConstants.vertBuf.vertices[gl_VertexIndex];

  mat4 view = global.view;
  view[3][0] = 0.0;
  view[3][1] = 0.0;
  view[3][2] = 0.0;

  texCoords = v.position;
  gl_Position = global.projection * view  * pushConstants.model * vec4(v.position, 1.0);
} 