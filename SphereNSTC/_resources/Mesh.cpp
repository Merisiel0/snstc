#include "Mesh.h"

#include "_graphics/Buffer.h"

#include <vector>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<Index> indices) {
  this->vertices = new Buffer(vertices,
    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
    VMA_MEMORY_USAGE_GPU_ONLY);

  this->indices = new Buffer(indices,
    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
    | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
    VMA_MEMORY_USAGE_GPU_ONLY);
}

Mesh::~Mesh() {
  delete vertices;
  delete indices;
}

Mesh* Mesh::generateCube() {
  std::vector<Vertex> vertices;
  for (float i = -0.5f; i <= 0.5f; i++) {
    for (float j = -0.5f; j <= 0.5f; j++) {
      for (float k = -0.5f; k <= 0.5f; k++) {
        Vertex v{};
        v.position = { i, j, k };
        v.color = { 1,1,1 };
        vertices.push_back(v);
      }
    }
  }

  std::vector<Index> indices = {
    0,1,2,2,1,3,
    1,5,3,3,5,7,
    5,4,7,7,4,6,
    4,0,6,6,0,2,
    5,1,4,4,1,0,
    6,2,7,7,2,3
  };

  return new Mesh(vertices, indices);
}
