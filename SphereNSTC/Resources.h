#pragma once

#include "Utils.h"
#include <vector>

struct Mesh {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
};

static Mesh generateCubeMesh() {
	Mesh m;

	for (float i = -0.5f; i < 0.5f; i++) {
		for (float j = -0.5f; j < 0.5f; j++) {
			for (float k = -0.5f; k < 0.5f; k++) {
				Vertex v{};
				v.position = { i, j, k };
				v.color = { 1,1,1,1 };
				m.vertices.push_back(v);
			}
		}
	}

	//todo: optimize with triangle strip
	m.indices.insert(m.indices.end(), {
		0,1,2,2,1,3,
		1,5,3,3,5,7,
		5,4,7,7,4,6,
		4,0,6,6,0,2,
		5,1,4,4,1,0,
		6,2,7,7,2,3
		});

	return m;
}