#pragma once

#include <vector>

class Image;

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
  std::vector<Image*> _maps{ MAP_COUNT, nullptr };

public:
  bool plainColor = true;

  Material() {}
  ~Material();

  Image* getColor() const { return _maps[ALBEDO]; }
  Image* getNormal() const { return _maps[NORMAL]; }
  Image* getRoughness() const { return _maps[ROUGHNESS]; }
  Image* getMetalness() const { return _maps[METALNESS]; }
  Image* getHeight() const { return _maps[HEIGHT]; }
  Image* getAO() const { return _maps[AMBIANT_OCCLUSION]; }

  static Material* load(const char* name);
};