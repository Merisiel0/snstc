#pragma once

#include <memory>
#include <vector>
#include <string>

class Image;
class ResourceManager;

enum MapIndex {
  ALBEDO = 0,
  NORMAL = 1,
  ROUGHNESS = 2,
  METALNESS = 3,
  HEIGHT = 4,
  AMBIENT_OCCLUSION = 5,
  MAP_COUNT = 6
};

class Material {
private:
  std::vector<std::shared_ptr<Image>> _maps {MAP_COUNT, nullptr};
  bool plainColor = true;

  Material(std::string path = "");

  friend class ResourceManager;

public:
  /// @brief Checks if this material is only a plain color.
  /// @return true if this material is a plain color, false otherwise.
  bool isPlainColor() { return plainColor; }

  /// @brief Checks if this material has a specific map.
  /// @param i a map index to check.
  /// @return true if this material has map of index i, false otherwise
  bool hasMap(MapIndex i) { return (bool)_maps[i]; }

  /// @brief Checks if this material has at least one map.
  /// @return true if this material has at least one map, false otherwise.
  bool hasAMap();

  /// @brief Gets one of the maps of this material.
  /// @param i a map index to return.
  /// @return the map at map index i if it exists, nullptr otherwise.
  Image& getMap(MapIndex i) { return *_maps[i]; }
};