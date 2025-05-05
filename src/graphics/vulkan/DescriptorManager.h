#pragma once

#include "DescriptorPool.h"
#include "DescriptorSet.h"
#include "DescriptorSetLayout.h"
#include "utils/Utils.h"

#include <array>

class Device;
class CommandBuffer;

/// @brief Types of available descriptor set layouts.
enum DescriptorSetLayoutType : uint8_t {
  /// @brief Global layout, contains camera and lighting buffers.
  DESCRIPTOR_SET_LAYOUT_GLOBAL = 0,
  /// @brief Skybox layout, contains a single albedo cubemap.
  DESCRIPTOR_SET_LAYOUT_SKYBOX = 1,
  /// @brief Material layout, contains all the PBR maps.
  DESCRIPTOR_SET_LAYOUT_MATERIAL,
  DESCRIPTOR_SET_LAYOUT_TYPE_COUNT
};

/// @brief Manager class to allocate and free descriptor pools, layouts and sets.
class DescriptorManager {
private:
  static std::shared_ptr<Device> _device;

  /// @brief Contains a list of pools for each layout type.
  static std::array<std::vector<std::shared_ptr<DescriptorPool>>, DESCRIPTOR_SET_LAYOUT_TYPE_COUNT>
    _pools;

  /// @brief Contains every layout objects.
  static std::array<std::unique_ptr<DescriptorSetLayout>, DESCRIPTOR_SET_LAYOUT_TYPE_COUNT>
    _layouts;

  /// @brief Contains a list of references to sets for each layout type.
  static std::array<std::vector<std::weak_ptr<DescriptorSet>>, DESCRIPTOR_SET_LAYOUT_TYPE_COUNT>
    _sets;

public:
  /// @brief Initializes initial descriptor pools and available layouts and allocates unique descriptor sets.
  /// @param device a device.
  static void init(std::shared_ptr<Device> device);

  /// @brief Frees all expired descriptor sets.
  static void cleanupExpired();

  /// @brief Frees all descriptor sets, layouts and pools.
  static void cleanup();

  /// @brief Gets a layout.
  /// @param type a layout type.
  /// @return a layout.
  static DescriptorSetLayout& getLayout(DescriptorSetLayoutType type);

  /// @brief Allocates a new descriptor set of the given type.
  /// @param commandBuffer a command buffer.
  /// @param type a descriptor set type.
  /// @return a new descriptor set.
  static std::shared_ptr<DescriptorSet> allocateSet(DescriptorSetLayoutType type);
};