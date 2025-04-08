#pragma once

#include "../graphics/vulkan/Image.h"
#include "Material.h"
#include "Mesh.h"

#include <memory>
#include <type_traits>
#include <unordered_map>
#include <vector>

class ResourceManager {
private:
  static std::unordered_map<const char*, std::weak_ptr<Image>> _images;
  static std::unordered_map<const char*, std::weak_ptr<Material>> _materials;
  static std::unordered_map<const char*, std::weak_ptr<Mesh>> _meshes;

public:
  static std::shared_ptr<Image> loadImage(const char* path);

  static std::shared_ptr<Material> loadMaterial(const char* path);

  static std::shared_ptr<Mesh> loadMesh(const char* path);
};