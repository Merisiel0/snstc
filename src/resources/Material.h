#pragma once

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

class Material {
private:
  std::vector<std::shared_ptr<Image>> _textures {TEXTURE_TYPE_COUNT, nullptr};
  std::vector<std::shared_ptr<DescriptorSet>> _descriptorSets;

  Material(std::string path = "");
  Material(Color color);

  friend class ResourceManager;

public:
  /// @brief Checks if this material has a specific map.
  /// @param i a map index to check.
  /// @return true if this material has map of index i, false otherwise
  bool hasMap(TextureType i) { return (bool) _textures[i]; }

  /// @brief Checks if this material has at least one map.
  /// @return true if this material has at least one map, false otherwise.
  bool hasAMap();

  /// @brief Gets one of the maps of this material.
  /// @param i a map index to return.
  /// @return the map at map index i if it exists, nullptr otherwise.
  Image& getMap(TextureType i);

  /// @brief Updates this material's descriptor set.
  /// @param sampler a sampler.
  void updateDescriptorSet(int frameIndex, const Sampler& sampler);

  /// @brief Gets this material's descriptor set.
  /// @return a descriptor set.
  std::shared_ptr<DescriptorSet> getDescriptorSet(int frameIndex) const;
};