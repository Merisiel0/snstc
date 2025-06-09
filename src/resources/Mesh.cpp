#include "Mesh.h"

#include "graphics/vulkan/Buffer.h"
#include "utils/Utils.h"

#include <sstream>
#include <vector>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<Index> indices, VkPrimitiveTopology topology) :
    _primitiveTopology {topology} {
  _vertices = new Buffer(vertices,
    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
      VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
    VMA_MEMORY_USAGE_GPU_ONLY);

  _indices = new Buffer(indices,
    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
      VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
    VMA_MEMORY_USAGE_GPU_ONLY);
}

const Buffer& Mesh::getVertices() const { return *_vertices; }

const Buffer& Mesh::getIndices() const { return *_indices; }

VkPrimitiveTopology Mesh::getPrimitiveTopology() const { return _primitiveTopology; }

// TODO implement reading of glb files
std::shared_ptr<Mesh> Mesh::load(std::string path) { return nullptr; }

Mesh::~Mesh() {
  delete _vertices;
  delete _indices;
}

// Mesh* Mesh::generateCube() {
//   // TODO: actual good procedural cube
//   std::vector<vec3> positions = {// 0,2,1
//     {-1.0f, 1.0f, -1.0f}, {-1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, -1.0f},
//     {1.0f, 1.0f, -1.0f}, {-1.0f, 1.0f, -1.0f},

//     {-1.0f, -1.0f, 1.0f}, {-1.0f, -1.0f, -1.0f}, {-1.0f, 1.0f, -1.0f}, {-1.0f, 1.0f, -1.0f},
//     {-1.0f, 1.0f, 1.0f}, {-1.0f, -1.0f, 1.0f},

//     {1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f},
//     {1.0f, 1.0f, -1.0f}, {1.0f, -1.0f, -1.0f},

//     {-1.0f, -1.0f, 1.0f}, {-1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f},
//     {1.0f, -1.0f, 1.0f}, {-1.0f, -1.0f, 1.0f},

//     {-1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f},
//     {-1.0f, 1.0f, 1.0f}, {-1.0f, 1.0f, -1.0f},

//     {-1.0f, -1.0f, -1.0f}, {-1.0f, -1.0f, 1.0f}, {1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, -1.0f},
//     {-1.0f, -1.0f, 1.0f}, {1.0f, -1.0f, 1.0f}};

//   std::vector<Vertex> vertices;
//   for(int i = 0; i < positions.size(); i++) {
//     Vertex v {};
//     v.position = positions[i];
//     vertices.push_back(v);
//   }

//   // std::vector<Vertex> vertices;
//   // for(float i = -0.5f; i <= 0.5f; i++) {
//   //   for(float j = -0.5f; j <= 0.5f; j++) {
//   //     for(float k = -0.5f; k <= 0.5f; k++) {
//   //       Vertex v {};
//   //       v.position = {i, j, k};
//   //       v.normal = normalize(v.position);
//   //       vertices.push_back(v);
//   //     }
//   //   }
//   // }

//   std::vector<Index> indices = {0, 1, 2, 2, 1, 3, 1, 5, 3, 3, 5, 7, 5, 4, 7, 7, 4, 6, 4, 0, 6, 6, 0,
//     2, 5, 1, 4, 4, 1, 0, 6, 2, 7, 7, 2, 3};

//   return new Mesh(vertices, indices);
// }

std::shared_ptr<Mesh> Mesh::generatePlane(vec2 dimensions, uvec2 vertexAmounts) {
  // generate path
  std::stringstream ss;
  ss << _generationPrefix << "plane" << dimensions << vertexAmounts;
  std::string path = ss.str();

  // return mesh if it already exists
  std::shared_ptr<Mesh> sptr = findResource(path);
  if(sptr) { return sptr; }

  // else generate a new one
  if(vertexAmounts.x < 2) vertexAmounts.x = 2;
  if(vertexAmounts.y < 2) vertexAmounts.y = 2;

  std::vector<Vertex> vertices;

  float halfWidth = dimensions.x / 2;
  float halfLength = dimensions.y / 2;
  float widthFoot = dimensions.x / (vertexAmounts.x - 1);
  float lengthFoot = dimensions.y / (vertexAmounts.y - 1);

  for(float i = -halfWidth; i <= halfWidth; i += widthFoot) {
    for(float j = -halfLength; j <= halfLength; j += lengthFoot) {
      Vertex v {};
      v.position = {i, 0, j};
      v.u = (i + halfWidth) / dimensions.x;
      v.v = (j + halfLength) / dimensions.y;
      v.normal = {0, 1, 0};

      vertices.push_back(v);
    }
  }

  std::vector<Index> indices;
  int maxI = (int) vertexAmounts.y * ((int) vertexAmounts.x - 1);
  for(int i = 0; i < maxI; i++) {
    if((i + 1) % (int) vertexAmounts.y != 0) {
      indices.insert(indices.end(), {i, i + (int) vertexAmounts.y, i + 1, i + (int) vertexAmounts.y,
                                      i + (int) vertexAmounts.y + 1, i + 1});
    }
  }

  sptr = std::make_shared<Mesh>(vertices, indices, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
  addResource(path, sptr);

  return sptr;
}

// Mesh* Mesh::generateCone(float radius, float height, int resolution) {
//   std::vector<Vertex> vertices;
//   std::vector<Index> indices;

//   // add vertices subdividing a circle
//   for(int i = 0; i < resolution; i++) {
//     float ratio = (float) i / resolution;
//     float r = ratio * glm::pi<float>() * 2.0f;
//     float x = std::cos(r) * radius;
//     float z = std::sin(r) * radius;

//     Vertex v {};
//     v.position = {x, 0, z};
//     vertices.push_back(v);
//   }

//   // add the tip of the cone
//   Vertex v {};
//   v.position = {0, 0, height};
//   Index tipIndex = (uint32_t) vertices.size();
//   vertices.push_back(v);

//   // add the bottom vertex
//   v.position = {0, 0, 0};
//   Index baseCenterIndex = (uint32_t) vertices.size();
//   vertices.push_back(v);

//   // generate triangular faces on side and bottow
//   for(int i = 0; i < resolution; i++) {
//     int ii = (i + 1) % resolution;
//     indices.insert(indices.begin(), {
//                                       tipIndex, ii, i, // side face
//                                       baseCenterIndex, i, ii // bottom face
//                                     });
//   }

//   return new Mesh(vertices, indices);
// }

// Mesh* Mesh::generateCylinder(float radius, float height, int resolution) {
//   std::vector<Vertex> vertices;
//   std::vector<Index> indices;

//   // generate vertices
//   for(size_t i = 0; i < resolution; i++) {
//     float ratio = (float) i / resolution;
//     float r = ratio * glm::pi<float>() * 2.0f;
//     float x = std::cos(r) * radius;
//     float y = std::sin(r) * radius;

//     Vertex v {};
//     v.position = {x, y, 0};
//     vertices.push_back(v);
//     v.position = {x, y, height};
//     vertices.push_back(v);
//   }

//   Vertex v {};
//   v.position = {0, 0, 0};
//   Index bottomCenterIndex = (uint32_t) vertices.size();
//   vertices.push_back(v);

//   v.position = {0, height, 0};
//   Index topCenterIndex = (uint32_t) vertices.size();
//   vertices.push_back(v);

//   // add triangles
//   for(int i = 0; i < resolution; i++) {
//     int ii = i * 2;
//     int jj = (ii + 2) % (resolution * 2);
//     int kk = (ii + 3) % (resolution * 2);
//     int ll = ii + 1;

//     indices.insert(indices.begin(), {
//                                       ii, ll, jj, // side triangles
//                                       jj, ll, kk, bottomCenterIndex, ii, jj, // bottom triangle
//                                       topCenterIndex, kk, ll // top triangle
//                                     });
//   }

//   return new Mesh(vertices, indices);
// }

// Mesh* Mesh::generateUVSphere(int nbSlices, int nbStacks) {
//   std::vector<Vertex> vertices;
//   std::vector<Index> indices;

//   // add top vertex
//   Vertex v0 {};
//   v0.position = {0, 1, 0};
//   vertices.push_back(v0);

//   // generate vertices per stack / slice
//   for(int i = 0; i < nbStacks - 1; i++) {
//     double phi = glm::pi<double>() * double(i + 1) / double(nbStacks);
//     for(int j = 0; j < nbSlices; j++) {
//       double theta = 2.0 * glm::pi<double>() * double(j) / double(nbSlices);
//       Vertex v1 {};
//       v1.position = {std::sin(phi) * std::cos(theta), std::cos(phi),
//         std::sin(phi) * std::sin(theta)};
//       vertices.push_back(v1);
//     }
//   }

//   // add bottom vertex
//   Vertex v1 {};
//   v1.position = {0, -1, 0};
//   int v1Index = (int) vertices.size();
//   vertices.push_back(v1);

//   // add top / bottom triangles
//   for(int i = 0; i < nbSlices; ++i) {
//     auto i0 = i + 1;
//     auto i1 = (i + 1) % nbSlices + 1;
//     indices.insert(indices.end(), {0, i1, i0});

//     i0 = i + nbSlices * (nbStacks - 2) + 1;
//     i1 = (i + 1) % nbSlices + nbSlices * (nbStacks - 2) + 1;
//     indices.insert(indices.end(), {v1Index, i0, i1});
//   }

//   // add quads per stack / slice
//   for(int j = 0; j < nbStacks - 2; j++) {
//     int j0 = j * nbSlices + 1;
//     int j1 = (j + 1) * nbSlices + 1;
//     for(int i = 0; i < nbSlices; i++) {
//       int i0 = j0 + i;
//       int i1 = j0 + (i + 1) % nbSlices;
//       int i2 = j1 + (i + 1) % nbSlices;
//       int i3 = j1 + i;
//       indices.insert(indices.end(), {i0, i1, i2, i2, i3, i0});
//     }
//   }

//   return new Mesh(vertices, indices);
// }

// Mesh* Mesh::generateIcoSphere(int nbDivisions) {
//   std::vector<Vertex> vertices;

//   float t = (1.0f + glm::sqrt(5.0f)) / 2.0f;

//   Vertex v {};
//   v.position = glm::normalize(vec3 {-1, t, 0});
//   vertices.push_back(v);
//   v.position = glm::normalize(vec3 {1, t, 0});
//   vertices.push_back(v);
//   v.position = glm::normalize(vec3 {-1, -t, 0});
//   vertices.push_back(v);
//   v.position = glm::normalize(vec3 {1, -t, 0});
//   vertices.push_back(v);
//   v.position = glm::normalize(vec3 {0, -1, t});
//   vertices.push_back(v);
//   v.position = glm::normalize(vec3 {0, 1, t});
//   vertices.push_back(v);
//   v.position = glm::normalize(vec3 {0, -1, -t});
//   vertices.push_back(v);
//   v.position = glm::normalize(vec3 {0, 1, -t});
//   vertices.push_back(v);
//   v.position = glm::normalize(vec3 {t, 0, -1});
//   vertices.push_back(v);
//   v.position = glm::normalize(vec3 {t, 0, 1});
//   vertices.push_back(v);
//   v.position = glm::normalize(vec3 {-t, 0, -1});
//   vertices.push_back(v);
//   v.position = glm::normalize(vec3 {-t, 0, 1});
//   vertices.push_back(v);

//   std::vector<Index> indices = {0, 11, 5, 0, 5, 1, 0, 1, 7, 0, 7, 10, 0, 10, 11, 1, 5, 9, 5, 11, 4,
//     11, 10, 2, 10, 7, 6, 7, 1, 8, 3, 9, 4, 3, 4, 2, 3, 2, 6, 3, 6, 8, 3, 8, 9, 4, 9, 5, 2, 4, 11, 6,
//     2, 10, 8, 6, 7, 9, 8, 1};

//   return new Mesh(vertices, indices);
// }