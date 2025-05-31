#pragma once

#include "Math.h"
#include "SDL3/SDL.h"
#include "Time.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#define SDL_CHECK(x)                                                                      \
  do {                                                                                    \
    if(!(bool) x) { std::cerr << "Detected SDL error: " << SDL_GetError() << std::endl; } \
  } while(0)

using Color = glm::vec4;

using Index = int;

struct Vertex {
  glm::vec3 position;
  float u;
  glm::vec3 normal;
  float v;
};

static inline std::string readFileText(std::string path) {
  std::ifstream file(path, std::ios::ate | std::ios::in);

  if(!file.is_open()) { throw std::runtime_error("Failed to read file text at:" + path); }

  size_t fileSize = (size_t) file.tellg();
  std::string text;

  file.seekg(0);
  file.read(text.data(), fileSize);
  file.close();

  return text;
}

static inline std::vector<uint32_t> readFileBytes(std::string path) {
  std::ifstream file(path, std::ios::ate | std::ios::in | std::ios::binary);

  if(!file.is_open()) { throw std::runtime_error("Failed to read file bytes at:" + path); }

  size_t fileSize = (size_t) file.tellg();
  std::vector<uint32_t> fileBytes(fileSize / sizeof(uint32_t));

  file.seekg(0);
  file.read((char*) fileBytes.data(), fileSize);
  file.close();

  return fileBytes;
}

inline std::ostream& operator<<(std::ostream& os, const glm::vec3& v) {
  os << "(" << v.x << "," << v.y << "," << v.z << ")";
  return os;
}

inline std::ostream& operator<<(std::ostream& os, const glm::vec2& v) {
  os << "(" << v.x << "," << v.y << ")";
  return os;
}

inline std::ostream& operator<<(std::ostream& os, const glm::mat4& m) {
  for(int i = 0; i < m.length(); i++) {
    for(int j = 0; j < m[i].length(); j++) {
      os << m[i][j] << " ";
    }
    os << "\n";
  }
  return os;
}

template<typename T>
inline std::shared_ptr<T> getShared(const std::weak_ptr<T>& weak) {
  if(!weak.expired()) {
    return weak.lock();
  } else {
    std::runtime_error("Weak pointer of type " + std::string(typeid(T).name()) + "has expired");
    return nullptr;
  }
}