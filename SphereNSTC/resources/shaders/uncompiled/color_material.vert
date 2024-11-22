#version 450
#extension GL_EXT_buffer_reference : require

layout (location = 0) out vec4 outColor;
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

layout( push_constant ) uniform constants {
	mat4 modelMatrix;
	VertexBuffer vertexBuffer;
} PushConstants;

void main()  {	
	Vertex v = PushConstants.vertexBuffer.vertices[gl_VertexIndex];
	gl_Position = cam.projView * PushConstants.modelMatrix * vec4(v.position, 1.0f);

	outColor = v.color;
	outFragPos = vec3(PushConstants.modelMatrix * vec4(v.position, 1.0));
	outNormal = mat3(transpose(inverse(PushConstants.modelMatrix))) * v.normal;
}