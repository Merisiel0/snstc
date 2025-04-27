#version 450
#extension GL_EXT_buffer_reference : require

layout(location = 0) out vec2 outUV;
layout(location = 1) out vec3 outFragPos;
layout(location = 2) out vec3 outNormal;

layout(std140, set = 0, binding = 0) uniform camBuf {
	mat4 projView;
} cam;

struct Vertex {
	vec3 position;
	float u; 
	vec3 normal;
	float v;
	vec4 color;
}; 

layout(buffer_reference, std430) readonly buffer VertexBuffer {
	Vertex vertices[];
};

struct Instance {
  mat4 model;
};

layout(buffer_reference, std430) readonly buffer InstanceBuffer {
  Instance instances[];
};

layout( push_constant ) uniform constants {
  mat4 transform;
	VertexBuffer vertexBuffer;
  InstanceBuffer instanceBuffer;
} PushConstants;

void main()  {	
	Vertex v = PushConstants.vertexBuffer.vertices[gl_VertexIndex];
  Instance instance = PushConstants.instanceBuffer.instances[gl_InstanceIndex];

  mat4 model = PushConstants.transform * instance.model;

	gl_Position = cam.projView * model * vec4(v.position, 1.0f);

	outUV = vec2(v.u, v.v);
	outFragPos = vec3(model * vec4(v.position, 1.0));
	outNormal = mat3(transpose(inverse(model))) * v.normal;
}