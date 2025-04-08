#include "ResourceManager.h"

#include <unordered_map>

std::unordered_map<const char*, std::weak_ptr<Image>> ResourceManager::_images;
std::unordered_map<const char*, std::weak_ptr<Mesh>> ResourceManager::_meshes;
std::unordered_map<const char*, std::weak_ptr<Material>> ResourceManager::_materials;

std::shared_ptr<Image> ResourceManager::loadImage(const char* path) {
  auto it = _images.find(path);
  if(it != _images.end() && !it->second.expired()) { return it->second.lock(); }

  std::shared_ptr<Image> img = std::shared_ptr<Image>(new Image(path));
  _images.insert({path, img});

  return img;
}

std::shared_ptr<Material> ResourceManager::loadMaterial(const char* path) {
  auto it = _materials.find(path);
  if(it != _materials.end() && !it->second.expired()) { return it->second.lock(); }

  std::shared_ptr<Material> mat = std::shared_ptr<Material>(new Material(path));
  _materials.insert({path, mat});

  return mat;
}

std::shared_ptr<Mesh> ResourceManager::loadMesh(const char* path) {
  auto it = _meshes.find(path);
  if(it != _meshes.end() && !it->second.expired()) { return it->second.lock(); }

  std::shared_ptr<Mesh> mesh = std::shared_ptr<Mesh>(new Mesh(path));
  _meshes.insert({path, mesh});

  return mesh;
}
