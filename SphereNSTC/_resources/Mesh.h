#pragma once

#include "_utils/Utils.h"
#include <vector>

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	static Mesh generateCube();
};