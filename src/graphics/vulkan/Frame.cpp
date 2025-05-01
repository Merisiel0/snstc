#include "Frame.h"

#include "CommandBuffer.h"
#include "CommandPool.h"
#include "DescriptorSet.h"
#include "DescriptorSetLayout.h"
#include "Device.h"
#include "Fence.h"
#include "Queue.h"
#include "Semaphore.h"

Frame::Frame(std::shared_ptr<Device> device, const DescriptorPool& pool,
  std::vector<std::shared_ptr<DescriptorSetLayout>> descriptorSetLayouts) {
  commandPool = std::make_shared<CommandPool>(device, device->graphicsQueue->familyIndex);
  commandBuffer = std::make_shared<CommandBuffer>(device, *commandPool);

  swapchainSemaphore = std::make_shared<Semaphore>(device);
  renderSemaphore = std::make_shared<Semaphore>(device);
  renderFence = std::make_shared<Fence>(device, true);

  for(const auto& setLayout : descriptorSetLayouts) {
    switch(setLayout->type) {
      case DESCRIPTOR_SET_LAYOUT_SCENE:
        sceneDescSet.reset(new DescriptorSet(device, pool, *setLayout));
        break;
      case DESCRIPTOR_SET_LAYOUT_SKYBOX:
        skyboxDescSet.reset(new DescriptorSet(device, pool, *setLayout));
        break;
      case DESCRIPTOR_SET_LAYOUT_OBJECT:
        objectDescSet.reset(new DescriptorSet(device, pool, *setLayout));
        break;
    }
  }
}

Frame::~Frame() {}