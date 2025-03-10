#pragma once

#include <memory>
#include <vector>

class Image;
class ResourceManager;

enum MapIndex {
  ALBEDO = 0,
  NORMAL = 1,
  ROUGHNESS = 2,
  METALNESS = 3,
  HEIGHT = 4,
  AMBIANT_OCCLUSION = 5,
  MAP_COUNT = 6
};

class Material {
 private:
  std::vector<std::shared_ptr<Image>> _maps{MAP_COUNT, nullptr};

  Material(const char* name);

  friend class ResourceManager;

 public:
  bool plainColor = true;

  bool hasMap(MapIndex i) { return !_maps[i]; }
  Image& getMap(MapIndex i) { return *_maps[i]; }
};