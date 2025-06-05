#include "DescriptorManager.h"

#include "CommandBuffer.h"
#include "Device.h"
#include "Swapchain.h"
#include "resources/Material.h"

std::array<std::vector<std::shared_ptr<DescriptorPool>>, DESCRIPTOR_SET_LAYOUT_TYPE_COUNT> DescriptorManager::_pools;
std::array<std::unique_ptr<DescriptorSetLayout>, DESCRIPTOR_SET_LAYOUT_TYPE_COUNT>
  DescriptorManager::_layouts;
std::array<std::vector<std::weak_ptr<DescriptorSet>>, DESCRIPTOR_SET_LAYOUT_TYPE_COUNT>
  DescriptorManager::_sets;

void DescriptorManager::init() {
  // global layout
  std::vector<VkDescriptorSetLayoutBinding> bindings;

  VkDescriptorSetLayoutBinding binding;
  binding.binding = 0; // camera buffer
  binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  binding.descriptorCount = 1;
  binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  binding.pImmutableSamplers = nullptr;
  bindings.push_back(binding);

  binding.binding = 1; // lights buffer
  binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  bindings.push_back(binding);

  _layouts[DESCRIPTOR_SET_LAYOUT_GLOBAL] =
    std::make_unique<DescriptorSetLayout>(bindings, DESCRIPTOR_SET_LAYOUT_GLOBAL);

  // skybox layout
  bindings.clear();

  binding.binding = 0; // cubemap
  binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  bindings.push_back(binding);

  _layouts[DESCRIPTOR_SET_LAYOUT_SKYBOX] =
    std::make_unique<DescriptorSetLayout>(bindings, DESCRIPTOR_SET_LAYOUT_SKYBOX);

  // material layout
  bindings.clear();

  binding.binding = 0; // albedo texture
  bindings.push_back(binding);

  binding.binding = 1; // normal texture
  bindings.push_back(binding);

  binding.binding = 2; // rougness texture
  bindings.push_back(binding);

  binding.binding = 3; // metalness texture
  bindings.push_back(binding);

  binding.binding = 4; // height texture
  bindings.push_back(binding);

  binding.binding = 5; // ambiant occlusion texture
  bindings.push_back(binding);

  _layouts[DESCRIPTOR_SET_LAYOUT_MATERIAL] =
    std::make_unique<DescriptorSetLayout>(bindings, DESCRIPTOR_SET_LAYOUT_MATERIAL);

  // allocate pools
  std::vector<VkDescriptorPoolSize> poolSizes = _layouts[DESCRIPTOR_SET_LAYOUT_GLOBAL]->getPoolSizes();
  _pools[DESCRIPTOR_SET_LAYOUT_GLOBAL].push_back(std::make_shared<DescriptorPool>(poolSizes));

  poolSizes = _layouts[DESCRIPTOR_SET_LAYOUT_SKYBOX]->getPoolSizes();
  _pools[DESCRIPTOR_SET_LAYOUT_SKYBOX].push_back(std::make_shared<DescriptorPool>(poolSizes));

  poolSizes = _layouts[DESCRIPTOR_SET_LAYOUT_MATERIAL]->getPoolSizes();
  _pools[DESCRIPTOR_SET_LAYOUT_MATERIAL].push_back(std::make_shared<DescriptorPool>(poolSizes));
}

void DescriptorManager::cleanupExpired() {
  for(auto& vec : _sets) {
    for(auto it = vec.begin(); it != vec.end(); it++) {
      if(it->expired()) { vec.erase(it); }
    }
  }
}

void DescriptorManager::cleanup() {
  for(auto& vec : _sets) {
    vec.clear();
  }

  for(auto& vec : _pools) {
    vec.clear();
  }

  for(auto& layout : _layouts) {
    layout.reset();
  }
}

DescriptorSetLayout& DescriptorManager::getLayout(DescriptorSetLayoutType type) {
  return *_layouts[type];
}

std::shared_ptr<DescriptorSet> DescriptorManager::allocateSet(DescriptorSetLayoutType type) {
  // find a pool with enough place for this set
  int poolIndex = -1;
  for(int i = 0; i < _pools[type].size(); i++) {
    int validSizes = 0;

    const auto freeSizes = _pools[type][i]->getFreeSizes();
    const auto neededSizes = _layouts[type]->getPoolSizes();
    for(const auto& freeSize : freeSizes) {
      for(const auto& neededSize : neededSizes) {
        if(freeSize.type == neededSize.type &&
           freeSize.descriptorCount >= neededSize.descriptorCount) {
          validSizes++;
          break;
        }
      }
    }

    if(validSizes == neededSizes.size()) {
      poolIndex = i;
      break;
    }
  }

  // if no pool has enough place, create new one with exponential space
  if(poolIndex == -1) {
    auto sizes = _pools[type].back()->getTotalSizes();
    for(auto& size : sizes) {
      size.descriptorCount *= 2;
    }

    _pools[type].push_back(std::make_shared<DescriptorPool>(sizes));
    poolIndex = _pools[type].size() - 1;
  }

  std::shared_ptr<DescriptorSet> set =
    std::make_shared<DescriptorSet>(_pools[type].at(poolIndex), *_layouts[type]);
  _sets[type].push_back(set);

  return set;
}