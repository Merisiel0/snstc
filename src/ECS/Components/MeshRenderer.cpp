#include "MeshRenderer.h"

#include "ECS/Components/Transform.h"
#include "ECS/GameObject.h"
#include "graphics/vulkan/Buffer.h"
#include "graphics/vulkan/GraphicsPipeline.h"
#include "resources/Material.h"

void MeshRenderer::updateInstanceBuffer(std::shared_ptr<CommandBuffer> commandBuffer) {
  if(isInstancing()){
    _instanceBuffer->update(commandBuffer, _instances);
  }
}

std::shared_ptr<Buffer> MeshRenderer::getInstancesBuffer() { return _instanceBuffer; }

int MeshRenderer::getMaxInstanceCount() { return _maxInstances; }

void MeshRenderer::setMaxInstanceCount(int max) {
  _maxInstances = max;

  if(max == 0) {
    _instanceBuffer.reset();
  } else {
    _instanceBuffer.reset(new Buffer(_maxInstances * sizeof(InstanceProperties),
      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
        VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
      VMA_MEMORY_USAGE_GPU_ONLY));
  }
}

int MeshRenderer::getInstanceCount() {
  if(isInstancing()) return 1;
  return (int) _instances.size(); 
}

void MeshRenderer::addInstance(InstanceProperties instance) {
  if(_instances.size() < _maxInstances) { _instances.push_back(instance); }
}

void MeshRenderer::clearInstances() { _instances.clear(); }

MeshRenderer::~MeshRenderer() {}
