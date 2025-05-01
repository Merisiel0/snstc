#include "Skybox.h"

#include "resources/ResourceManager.h"

Skybox::Skybox(std::vector<std::string> cubemapFaces) {
  _mesh = ResourceManager::generateCube();
  _cubemap = ResourceManager::loadImage(cubemapFaces);
}

std::shared_ptr<Mesh> Skybox::getMesh() const { return _mesh; }

std::shared_ptr<Image> Skybox::getCubeMap() const { return _cubemap; }
