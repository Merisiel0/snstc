#include "Material.h"

#include "_graphics/_vulkan/Image.h"

#include <filesystem>
#include <string>
#include <vector>

Material::~Material() {
  for (int i = 0; i < MAP_COUNT; i++) {
    delete _maps[i];
  }
}

Material* Material::load(const char* path) {
  if (!path) return nullptr;

  std::vector<const char*> mapNames = {
    "/color",
    //"/normal",
    //"/roughness",
    //"/metalness",
    //"/height",
    //"/ao"
  };

  std::vector<const char*> supportedImageExtensions = {
    ".png", ".jpg"
  };

  Material* mat = new Material();
  for (int i = 0; i < mapNames.size(); i++) {
    for (int j = 0; j < supportedImageExtensions.size(); j++) {
      std::string completePath = path;
      completePath.append(mapNames[i]);
      completePath.append(supportedImageExtensions[j]);
      if (std::filesystem::exists(completePath)) {
        mat->_maps[i] = new Image(completePath.c_str());
        mat->plainColor = false;
      }
    }
  }

  return mat;
}
