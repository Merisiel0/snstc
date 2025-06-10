#pragma once

#include <utility>
#include <vector>

template<typename T>
inline void hashCombine(std::size_t& seed, const T& val) {
  seed ^= std::hash<T>(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template<typename T>
inline std::size_t hashVectorUnordered(const std::vector<T>& vec) {
  std::size_t hash = 0;
  for(const auto& item : vec) {
    std::size_t h = std::hash<T> {}(item);
    hash += h * h + 0x9e3779b9;
  }
  return hash;
}