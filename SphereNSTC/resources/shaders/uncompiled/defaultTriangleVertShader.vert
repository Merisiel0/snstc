#version 450
#extension GL_EXT_buffer_reference : require

layout (location = 0) out vec4 outColor;

struct Vertex {
	vec3 color;
	vec3 normal;
	vec3 position;
	vec2 uv;
}; 

layout(buffer_reference, std430) readonly buffer VertexBuffer{ 
	Vertex vertices[];
};

//push constants block
layout( push_constant ) uniform constants
{	
	mat4 worldMatrix;
	VertexBuffer vertexBuffer;
} PushConstants;


//// TEMP
//const vec3 positions[3] = vec3[3](
//	vec3(1.f,1.f, 0.0f),
//	vec3(-1.f,1.f, 0.0f),
//	vec3(0.f,-1.f, 0.0f)
//);
//
//// TEMP
//const vec4 colors[3] = vec4[3](
//	vec4(1.0f, 0.0f, 0.0f, 1.0f), //red
//	vec4(0.0f, 1.0f, 0.0f, 1.0f), //green
//	vec4(00.f, 0.0f, 1.0f, 1.0f)  //blue
//);

void main() 
{	
	//load vertex data from device adress
	Vertex v = PushConstants.vertexBuffer.vertices[gl_VertexIndex];

	//output data
	gl_Position = PushConstants.worldMatrix * vec4(v.position, 1.0f);
	outColor = vec4(v.color, 1.0f);
	
//	gl_Position =  vec4(positions[gl_VertexIndex], 1.0f);
//	outColor = colors[gl_VertexIndex];
}