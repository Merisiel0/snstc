#include "Material.h"

#include <filesystem>
#include <string>
#include <vector>

#include "graphics/vulkan/Image.h"
#include "ResourceManager.h"

Material::Material(const char* path) {
  std::vector<const char*> mapNames = {
      "/color",
      //"/normal",
      //"/roughness",
      //"/metalness",
      //"/height",
      //"/ao"
  };

  std::vector<const char*> supportedImageExtensions = {".png", ".jpg"};

  for (int i = 0; i < mapNames.size(); i++) {
    for (int j = 0; j < supportedImageExtensions.size(); j++) {
      std::string completePath = path;
      completePath.append(mapNames[i]);
      completePath.append(supportedImageExtensions[j]);
      if (std::filesystem::exists(completePath)) {
        _maps[i] = ResourceManager::loadImage(completePath.c_str());
        plainColor = false;
      }
    }
  }
}
