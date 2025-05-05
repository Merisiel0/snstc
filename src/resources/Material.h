#pragma once

#include "utils/Utils.h"

#include <memory>
#include <string>
#include <vector>

class Image;
class ResourceManager;
class DescriptorSet;
class Sampler;

enum MapIndex {
  MAP_INDEX_ALBEDO = 0,
  MAP_INDEX_NORMAL = 1,
  MAP_INDEX_ROUGHNESS = 2,
  MAP_INDEX_METALNESS = 3,
  MAP_INDEX_HEIGHT = 4,
  MAP_INDEX_AMBIENT_OCCLUSION = 5,
  MAP_INDEX_COUNT = 6
};

class Material {
private:
  std::vector<std::shared_ptr<Image>> _maps {MAP_INDEX_COUNT, nullptr};
  std::vector<std::shared_ptr<DescriptorSet>> _descriptorSets;

  Material(std::string path = "");
  Material(Color color);

  friend class ResourceManager;

public:
  /// @brief Checks if this material has a specific map.
  /// @param i a map index to check.
  /// @return true if this material has map of index i, false otherwise
  bool hasMap(MapIndex i) { return (bool) _maps[i]; }

  /// @brief Checks if this material has at least one map.
  /// @return true if this material has at least one map, false otherwise.
  bool hasAMap();

  /// @brief Gets one of the maps of this material.
  /// @param i a map index to return.
  /// @return the map at map index i if it exists, nullptr otherwise.
  Image& getMap(MapIndex i);

  /// @brief Updates this material's descriptor set.
  /// @param sampler a sampler.
  void updateDescriptorSet(int frameIndex, const Sampler& sampler);

  /// @brief Gets this material's descriptor set.
  /// @return a descriptor set.
  std::shared_ptr<DescriptorSet> getDescriptorSet(int frameIndex) const;
};