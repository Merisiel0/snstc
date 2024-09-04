#pragma once

#include <vector>

class GraphicsPipeline;
class Image;

class Material {
private:
  enum MapIndex {
    ALBEDO = 0,
    NORMAL = 1,
    ROUGHNESS = 2,
    METALNESS = 3,
    HEIGHT = 4,
    AMBIANT_OCCLUSION = 5,
    MAP_COUNT = AMBIANT_OCCLUSION
  };

  std::vector<Image*> _maps{ 5, nullptr };

public:
  GraphicsPipeline* pipeline;

  Material(GraphicsPipeline* pipeline) : pipeline{ pipeline } {};
  Material(GraphicsPipeline* pipeline,
    const char* albedoMapPath,
    const char* normalMapPath,
    const char* rougnessMapPath,
    const char* metalnessMapPath,
    const char* heightMapPath,
    const char* aoMapPath);
  ~Material();

  static Material* load(GraphicsPipeline* pipeline, const char* name);
};