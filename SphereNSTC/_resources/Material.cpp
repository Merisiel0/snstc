#include "Material.h"

#include "_graphics/Image.h"

#include <filesystem>
#include <string>
#include <vector>

Material::Material(GraphicsPipeline* pipeline,
  const char* albedoMapPath,
  const char* normalMapPath,
  const char* rougnessMapPath,
  const char* metalnessMapPath,
  const char* heightMapPath,
  const char* aoMapPath) {

  this->pipeline = pipeline;

  std::vector<const char*> paths = {
    albedoMapPath,
    normalMapPath,
    rougnessMapPath,
    metalnessMapPath,
    heightMapPath,
    aoMapPath
  };

  for (int i = 0; i < MAP_COUNT; i++) {
    if (paths[i]) _maps[i] = new Image(paths[i]);
  }
}

Material::~Material() {
  for (int i = 0; i < MAP_COUNT; i++) {
    delete _maps[i];
  }
}

Material* Material::load(GraphicsPipeline* pipeline, const char* name) {
  Material* mat = new Material(pipeline);

  std::vector<const char*> pathsEnds = {
    "/color.jpg",
    "/normal.jpg",
    "/roughness.jpg",
    "/metalness.jpg",
    "/height.jpg",
    "/ao.jpg"
  };

  for (int i = 0; i < MAP_COUNT; i++) {
    const char* path = std::string(name).append(pathsEnds[i]).c_str();
    if (std::filesystem::exists(path))
      mat->_maps[i] = new Image(path);
  }

  return mat;
}
