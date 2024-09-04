#pragma once

#include "_utils/Utils.h"
#include <vector>

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<Index> indices;

	static Mesh generateCube();
};