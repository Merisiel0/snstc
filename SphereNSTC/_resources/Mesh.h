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

	static Mesh* generateCube();
};