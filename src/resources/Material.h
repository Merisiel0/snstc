#pragma once

#include "Resource.h"
#include "utils/Utils.h"

#include <memory>
#include <string>
#include <vector>

class Image;
class ResourceManager;
class DescriptorSet;
class Sampler;

enum TextureType {
  TEXTURE_ALBEDO = 0,
  TEXTURE_NORMAL = 1,
  TEXTURE_ROUGHNESS = 2,
  TEXTURE_METALNESS = 3,
  TEXTURE_HEIGHT = 4,
  TEXTURE_AMBIENT_OCCLUSION = 5,
  TEXTURE_TYPE_COUNT = 6
};

class Material : public Resource<Material, std::string> {
private:
  std::vector<std::shared_ptr<Image>> _textures {TEXTURE_TYPE_COUNT, nullptr};
  std::vector<std::shared_ptr<DescriptorSet>> _descriptorSets;

  Material();

public:
  /// @brief Loads a material from a given path.
  /// @param path a path to a material asset.
  /// @return a shared pointer to the material if the path exists, nullptr otherwise.
  static std::shared_ptr<Material> load(std::string path);

  /// @brief Loads a material from a given color.
  /// @param color a color.
  /// @return a shared pointer to the material.
  static std::shared_ptr<Material> load(Color color);

  /// @brief Checks if this material has at least one texture.
  /// @return true if this material has at least one texture, false otherwise.
  bool hasTextures();

  /// @brief Gets this material's descriptor set.
  /// @return a descriptor set.
  std::shared_ptr<DescriptorSet> getDescriptorSet(int frameIndex) const;
};