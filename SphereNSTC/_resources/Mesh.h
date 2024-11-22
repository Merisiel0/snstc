#pragma once

#include "_utils/Utils.h"

#include <vector>

class Buffer;

class Mesh {
public:
	Buffer* vertices;
	Buffer* indices;

	Mesh(std::vector<Vertex> vertices, std::vector<Index> indices);
	~Mesh();

	static Mesh* generateCube(Color color = { 0,0,0,1 });
	static Mesh* generatePlane(vec2 dimensions, vec2 vertexAmounts, Color color = { 0,0,0,1 });
	static Mesh* generateCone(float radius, float height, int resolution, Color color = { 0,0,0,1 });
	static Mesh* generateCylinder(float radius, float height, int resolution, Color color = { 0,0,0,1 });

	static Mesh* generateUVSphere(int nbSlices, int nbStacks, Color color = { 0,0,0,1 });
	static Mesh* generateIcoSphere(int nbDivisions, Color color = { 0,0,0,1 });
};