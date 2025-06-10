#pragma once

#include "Resource.h"
#include "graphics/vulkan/VulkanUtils.h"
#include "utils/Utils.h"

#include <vector>

class Buffer;
class ResourceManager;

class Mesh : public Resource<Mesh, std::string> {
private:
  const Buffer* _vertices;
  const Buffer* _indices;
  const VkPrimitiveTopology _primitiveTopology;

  Mesh(std::vector<Vertex> vertices, std::vector<Index> indices, VkPrimitiveTopology topology);

public:
  const Buffer& getVertices() const;
  const Buffer& getIndices() const;
  VkPrimitiveTopology getPrimitiveTopology() const;

  static std::shared_ptr<Mesh> load(std::string path);

  // TODO finish implementing procedural primitives
  // static Mesh* generateCube();
  static std::shared_ptr<Mesh> generatePlane(vec2 dimensions, uvec2 vertexAmounts);
  // static Mesh* generateCone(float radius, float height, int resolution);
  // static Mesh* generateCylinder(float radius, float height, int resolution);

  // static Mesh* generateUVSphere(int nbSlices, int nbStacks);
  // static Mesh* generateIcoSphere(int nbDivisions);

  ~Mesh();
};