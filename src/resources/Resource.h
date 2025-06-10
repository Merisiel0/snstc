#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

static std::string assetsPath;

template<typename T, typename U>
class Resource {
private:
  /// @brief A list of all instantiated resources.
  static std::unordered_map<U, std::weak_ptr<T>> _resources;

  /// @brief Wether or not the list contains expired resources.
  static bool _containsExpiredResources;

protected:
  /// @brief In order to have unique ids, this prefix is put in front of every dynamically generated resources.
  ///
  /// This means no user provided resource should start with this prefix.
  static constexpr const char* _generationPrefix = "dgyenn";

  /// @brief finds a resource if it already exists.
  /// @param path the path of the resource.
  /// @return a shared pointer to the resource if it exists, nullptr otherwise.
  static std::shared_ptr<T> findResource(U key);

  /// @brief Registers a resource in the list of allocated resources.
  /// @param path the path of the resource.
  /// @param resource the resource.
  static void addResource(U key, std::weak_ptr<T> resource);

public:
  Resource();
  ~Resource();

  // A resource should never be copied.
  Resource(const Resource<T, U>& other) = delete;
  Resource& operator=(const Resource<T, U>& other) = delete;

  // A resource should never be moved.
  Resource(Resource<T, U>&& other) = delete;
  Resource& operator=(Resource<T, U>&& other) = delete;

  /// @brief Cleans up pointers to expired resources.
  static void cleanupExpired();

  /// @brief Cleans up every allocated resource.
  static void cleanup();
};

template<typename T, typename U>
inline std::shared_ptr<T> Resource<T, U>::findResource(U key) {
  auto it = _resources.find(key);
  if(it != _resources.end()) {
    if(it->second.expired()) {
      _resources.erase(key);
      return nullptr;
    } else {
      return it->second.lock();
    }
  }
  return nullptr;
}

template<typename T, typename U>
inline void Resource<T, U>::addResource(U key, std::weak_ptr<T> resource) {
  _resources.insert({key, resource});
}

template<typename T, typename U>
inline Resource<T, U>::Resource() {}

template<typename T, typename U>
inline Resource<T, U>::~Resource() {
  _containsExpiredResources = true;
}

template<typename T, typename U>
inline void Resource<T, U>::cleanupExpired() {
  if(_containsExpiredResources) {
    for(auto it = _resources.begin(); it != _resources.end(); it++) {
      if((*it).second.expired()) { _resources.erase(it); }
    }
    _containsExpiredResources = false;
  }
}

template<typename T, typename U>
inline void Resource<T, U>::cleanup() {
  _resources.clear();
}