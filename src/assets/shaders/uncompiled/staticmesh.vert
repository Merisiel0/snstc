#version 450
#extension GL_EXT_buffer_reference : require

layout(location = 0) out vec2 outUV;

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

// main shader
void main()  {	
  Vertex v = pushConstants.vertBuf.vertices[gl_VertexIndex];

	outUV = vec2(v.u, v.v);
  gl_Position = global.projection * global.view  * pushConstants.model * vec4(v.position, 1.0);
}