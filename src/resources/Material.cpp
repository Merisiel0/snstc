#include "Material.h"

#include "ResourceManager.h"
#include "graphics/vulkan/DescriptorManager.h"
#include "graphics/vulkan/Image.h"
#include "graphics/vulkan/Swapchain.h"

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
        _textures[i] = ResourceManager::loadImage(completePath.c_str());
      }
    }
  }

  if(!hasAMap()) {
    std::cerr
      << "PBR Material initialized without any maps. This shouldn't happen and is probably a path issue.";
  }

  for(int i = 0; i < Swapchain::FRAME_OVERLAP; i++){
    _descriptorSets.push_back(DescriptorManager::allocateSet(DESCRIPTOR_SET_LAYOUT_MATERIAL));
  }
}

Material::Material(Color color) {
  _textures[TEXTURE_ALBEDO] = ResourceManager::loadImage(1, 1, color);
  
  for(int i = 0; i < Swapchain::FRAME_OVERLAP; i++){
    _descriptorSets.push_back(DescriptorManager::allocateSet(DESCRIPTOR_SET_LAYOUT_MATERIAL));
  }
}

bool Material::hasAMap() {
  for(int i = 0; i < TextureType::TEXTURE_TYPE_COUNT; i++) {
    if(hasMap((TextureType) i)) { return true; }
  }
  return false;
}

Image& Material::getMap(TextureType i) { return *_textures[i]; }

void Material::updateDescriptorSet(int frameIndex, const Sampler& sampler) {
  for(int i = 0; i < TEXTURE_TYPE_COUNT; i++) {
    if(_textures[i]) {
      _descriptorSets.at(frameIndex)->write(i, getMap((TextureType) i), sampler);
    }
  }
}

std::shared_ptr<DescriptorSet> Material::getDescriptorSet(int frameIndex) const { return _descriptorSets.at(frameIndex); }
