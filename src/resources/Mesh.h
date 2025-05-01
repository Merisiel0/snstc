#pragma once

#include "utils/Utils.h"

#include <vector>

class Buffer;
class ResourceManager;

class Mesh {
private:
  Mesh(std::string path) {}
  Mesh(std::vector<Vertex> vertices, std::vector<Index> indices);

  friend class ResourceManager;

  static Mesh* generateCube();
  static Mesh* generatePlane(vec2 dimensions, vec2 vertexAmounts);
  static Mesh* generateCone(float radius, float height, int resolution);
  static Mesh* generateCylinder(float radius, float height, int resolution);

  static Mesh* generateUVSphere(int nbSlices, int nbStacks);
  static Mesh* generateIcoSphere(int nbDivisions);

public:
  Buffer* vertices;
  Buffer* indices;

  ~Mesh();
};