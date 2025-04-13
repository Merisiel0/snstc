#include "Material.h"

#include "ResourceManager.h"
#include "graphics/vulkan/Image.h"

#include <filesystem>
#include <string>
#include <vector>

Material::Material(std::string path) {
  if(path.empty()) return;

  std::vector<const char*> mapNames = {
    "/color",
    //"/normal",
    //"/roughness",
    //"/metalness",
    //"/height",
    //"/ao"
  };

  std::vector<const char*> supportedImageExtensions = {".png", ".jpg"};

  for(int i = 0; i < mapNames.size(); i++) {
    for(int j = 0; j < supportedImageExtensions.size(); j++) {
      std::string completePath = path;
      completePath.append(mapNames[i]);
      completePath.append(supportedImageExtensions[j]);
      if(std::filesystem::exists(completePath)) {
        _maps[i] = ResourceManager::loadImage(completePath.c_str());
        if(i == 0) { plainColor = false; }
      }
    }
  }

  if(!hasAMap()){
    std::cerr << "PBR Material initialized without any maps. This shouldn't happen and is probably a path issue.";
  }
}

bool Material::hasAMap() {
  for(int i = 0; i < MapIndex::MAP_COUNT; i++) {
    if(hasMap((MapIndex) i)) { return true; }
  }
  return false;
}
