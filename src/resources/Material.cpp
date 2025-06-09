#include "Material.h"

#include "ResourceManager.h"
#include "graphics/vulkan/DescriptorManager.h"
#include "graphics/vulkan/Image.h"
#include "graphics/vulkan/Sampler.h"
#include "graphics/vulkan/Swapchain.h"
#include "graphics/vulkan/VulkanHandler.h"
#include "graphics/vulkan/VulkanUtils.h"

#include <filesystem>
#include <sstream>
#include <string>
#include <vector>

Material::Material() {}

std::shared_ptr<Material> Material::load(std::string path) {
  // return material if it already exists, else generate a new one
  std::shared_ptr<Material> sptr = findResource(path);
  if(sptr) { return sptr; }

  // check if path exists
  if(!std::filesystem::exists(path)) {
    std::clog << "Material not found at path." << std::endl;
    return nullptr;
  }

  // texture names
  std::vector<const char*> mapNames = {
    "/color",
    //"/normal",
    //"/roughness",
    //"/metalness",
    //"/height",
    //"/ao"
  };

  // supported texture extensions
  std::vector<const char*> supportedImageExtensions = {".png", ".jpg"};

  // load textures
  for(int i = 0; i < mapNames.size(); i++) {
    for(int j = 0; j < supportedImageExtensions.size(); j++) {
      std::string completePath = path;
      completePath.append(mapNames[i]);
      completePath.append(supportedImageExtensions[j]);
      if(std::filesystem::exists(completePath)) {
        sptr->_textures[i] = ResourceManager::loadImage(completePath.c_str());
      }
    }
  }

  // check if textures loaded correctly
  if(!sptr->hasTextures()) {
    std::cerr
      << "PBR Material initialized without any maps. This shouldn't happen and is probably a path issue.";
  }

  // create descriptor sets
  for(int i = 0; i < Swapchain::FRAME_OVERLAP; i++) {
    sptr->_descriptorSets.push_back(DescriptorManager::allocateSet(DESCRIPTOR_SET_LAYOUT_MATERIAL));
    for(int j = 0; j < TEXTURE_TYPE_COUNT; j++) {
      sptr->_descriptorSets.back()->write(j, *sptr->_textures[j], VulkanHandler::getSampler());
    }
  }

  addResource(path, sptr);

  return sptr;
}

std::shared_ptr<Material> Material::load(Color color) {
  // generate path
  std::stringstream ss;
  ss << _generationPrefix << "color" << color;
  std::string path = ss.str();

  // return material if it already exists
  std::shared_ptr<Material> sptr = findResource(path);
  if(sptr) { return sptr; }

  // else generate a new one
  sptr = std::make_shared<Material>();

  sptr->_textures[TEXTURE_ALBEDO] = ResourceManager::loadImage(1, 1, color);

  for(int i = 0; i < Swapchain::FRAME_OVERLAP; i++) {
    sptr->_descriptorSets.push_back(DescriptorManager::allocateSet(DESCRIPTOR_SET_LAYOUT_MATERIAL));
    sptr->_descriptorSets.back()->write(0, *sptr->_textures[0], VulkanHandler::getSampler());
  }

  addResource(path, sptr);

  return sptr;
}

bool Material::hasTextures() {
  for(int i = 0; i < TextureType::TEXTURE_TYPE_COUNT; i++) {
    if(_textures[i]) { return true; }
  }
  return false;
}

std::shared_ptr<DescriptorSet> Material::getDescriptorSet(int frameIndex) const {
  return _descriptorSets.at(frameIndex);
}
