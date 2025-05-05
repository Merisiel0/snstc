#include "ResourceManager.h"

#include <filesystem>
#include <sstream>
#include <unordered_map>

std::unordered_map<std::string, std::weak_ptr<Image>> ResourceManager::_images;
std::unordered_map<std::string, std::weak_ptr<Mesh>> ResourceManager::_meshes;
std::unordered_map<std::string, std::weak_ptr<Material>> ResourceManager::_materials;

std::string ResourceManager::assetsPath = "";

std::string ResourceManager::getCompletePath(std::string partialPath) {
  return assetsPath + partialPath;
}

void ResourceManager::cleanupExpired() {
  std::unordered_map<std::string, std::weak_ptr<Image>>::iterator it0;
  for(it0 = _images.begin(); it0 != _images.end(); it0++) {
    if((*it0).second.expired()) { _images.erase(it0); }
  }

  std::unordered_map<std::string, std::weak_ptr<Mesh>>::iterator it1;
  for(it1 = _meshes.begin(); it1 != _meshes.end(); it1++) {
    if((*it1).second.expired()) { _meshes.erase(it1); }
  }

  std::unordered_map<std::string, std::weak_ptr<Material>>::iterator it2;
  for(it2 = _materials.begin(); it2 != _materials.end(); it2++) {
    if((*it2).second.expired()) { _materials.erase(it2); }
  }
}

std::shared_ptr<Image> ResourceManager::loadImage(std::string path) {
  auto it = _images.find(path);
  if(it != _images.end()) {
    if(it->second.expired()) {
      _images.erase(path);
    } else {
      return it->second.lock();
    }
  }

  if(!std::filesystem::exists(path)) { return nullptr; }

  std::shared_ptr<Image> img = std::shared_ptr<Image>(new Image(path));
  _images.insert({path, img});

  return img;
}

std::shared_ptr<Image> ResourceManager::loadImage(std::vector<std::string> paths) {
  if(paths.size() < 6) return nullptr;

  std::string pathKey = "";
  for(int i = 0; i < 6; i++) {
    if(!std::filesystem::exists(paths[i])) return nullptr;
    pathKey.append(paths[i]);
  }

  auto it = _images.find(pathKey);
  if(it != _images.end()) {
    if(it->second.expired()) {
      _images.erase(pathKey);
    } else {
      return it->second.lock();
    }
  }

  std::shared_ptr<Image> img = std::shared_ptr<Image>(new Image(paths));
  _images.insert({pathKey, img});

  return img;
}

std::shared_ptr<Image> ResourceManager::loadImage(int width, int height, Color color) {
  std::stringstream ss;
  ss << _generationPrefix << "image" << width << height << color.r << color.g << color.b << color.a;

  std::string path = ss.str();

  auto it = _images.find(path);
  if(it != _images.end()) {
    if(it->second.expired()) {
      _images.erase(path);
    } else {
      return it->second.lock();
    }
  }

  std::shared_ptr<Image> img = std::shared_ptr<Image>(new Image(width, height, color));
  _images.insert({path, img});

  return img;
}

std::shared_ptr<Material> ResourceManager::loadMaterial(std::string path) {
  auto it = _materials.find(path);
  if(it != _materials.end()) {
    if(it->second.expired()) {
      _materials.erase(path);
    } else {
      return it->second.lock();
    }
  }

  if(!std::filesystem::exists(path)) { return nullptr; }

  std::shared_ptr<Material> mat = std::shared_ptr<Material>(new Material(path));
  _materials.insert({path, mat});

  return mat;
}

std::shared_ptr<Material> ResourceManager::loadMaterial(Color color) {
  std::stringstream ss;
  ss << _generationPrefix << "color" << color.r << color.g << color.b << color.a;

  std::string path = ss.str();

  auto it = _materials.find(path);
  if(it != _materials.end()) {
    if(it->second.expired()) {
      _materials.erase(path);
    } else {
      return it->second.lock();
    }
  }

  std::shared_ptr<Material> mat = std::shared_ptr<Material>(new Material(color));
  _materials.insert({path, mat});

  return mat;
}

std::shared_ptr<Mesh> ResourceManager::loadMesh(std::string path) {
  auto it = _meshes.find(path);
  if(it != _meshes.end()) {
    if(it->second.expired()) {
      _meshes.erase(path);
    } else {
      return it->second.lock();
    }
  }

  std::shared_ptr<Mesh> mesh = std::shared_ptr<Mesh>(new Mesh(path));
  _meshes.insert({path, mesh});

  return mesh;
}

std::shared_ptr<Mesh> ResourceManager::generateCube() {
  std::stringstream ss;
  ss << _generationPrefix << "cube";

  std::string path = ss.str();

  auto it = _meshes.find(path);
  if(it != _meshes.end()) {
    if(it->second.expired()) {
      _meshes.erase(path);
    } else {
      return it->second.lock();
    }
  }

  std::shared_ptr<Mesh> mesh = std::shared_ptr<Mesh>(Mesh::generateCube());
  _meshes.insert({path, mesh});

  return mesh;
}

std::shared_ptr<Mesh> ResourceManager::generatePlane(vec2 dimensions, vec2 vertexAmounts) {
  std::stringstream ss;
  ss << _generationPrefix << "plane" << dimensions.x << dimensions.y << vertexAmounts.x
     << vertexAmounts.y;

  std::string path = ss.str();

  auto it = _meshes.find(path);
  if(it != _meshes.end()) {
    if(it->second.expired()) {
      _meshes.erase(path);
    } else {
      return it->second.lock();
    }
  }

  std::shared_ptr<Mesh> mesh =
    std::shared_ptr<Mesh>(Mesh::generatePlane(dimensions, vertexAmounts));
  _meshes.insert({path, mesh});

  return mesh;
}

std::shared_ptr<Mesh> ResourceManager::generateCone(float radius, float height, int resolution) {
  std::stringstream ss;
  ss << _generationPrefix << "cone" << radius << height << resolution;

  std::string path = ss.str();

  auto it = _meshes.find(path);
  if(it != _meshes.end()) {
    if(it->second.expired()) {
      _meshes.erase(path);
    } else {
      return it->second.lock();
    }
  }

  std::shared_ptr<Mesh> mesh =
    std::shared_ptr<Mesh>(Mesh::generateCone(radius, height, resolution));
  _meshes.insert({path, mesh});

  return mesh;
}

std::shared_ptr<Mesh> ResourceManager::generateCylinder(float radius, float height,
  int resolution) {
  std::stringstream ss;
  ss << _generationPrefix << "cylinder" << radius << height << resolution;

  std::string path = ss.str();

  auto it = _meshes.find(path);
  if(it != _meshes.end()) {
    if(it->second.expired()) {
      _meshes.erase(path);
    } else {
      return it->second.lock();
    }
  }

  std::shared_ptr<Mesh> mesh =
    std::shared_ptr<Mesh>(Mesh::generateCylinder(radius, height, resolution));
  _meshes.insert({path, mesh});

  return mesh;
}

std::shared_ptr<Mesh> ResourceManager::generateUVSphere(int nbSlices, int nbStacks) {
  std::stringstream ss;
  ss << _generationPrefix << "cylinder" << nbSlices << nbStacks;

  std::string path = ss.str();

  auto it = _meshes.find(path);
  if(it != _meshes.end()) {
    if(it->second.expired()) {
      _meshes.erase(path);
    } else {
      return it->second.lock();
    }
  }

  std::shared_ptr<Mesh> mesh = std::shared_ptr<Mesh>(Mesh::generateUVSphere(nbSlices, nbStacks));
  _meshes.insert({path, mesh});

  return mesh;
}

std::shared_ptr<Mesh> ResourceManager::generateIcoSphere(int nbDivisions) {
  std::stringstream ss;
  ss << _generationPrefix << "cylinder" << nbDivisions;

  std::string path = ss.str();

  auto it = _meshes.find(path);
  if(it != _meshes.end()) {
    if(it->second.expired()) {
      _meshes.erase(path);
    } else {
      return it->second.lock();
    }
  }

  std::shared_ptr<Mesh> mesh = std::shared_ptr<Mesh>(Mesh::generateIcoSphere(nbDivisions));
  _meshes.insert({path, mesh});

  return mesh;
}
