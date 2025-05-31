#include "Skybox.h"

#include "graphics/vulkan/DescriptorManager.h"
#include "graphics/vulkan/GraphicsPipelineId.h"
#include "graphics/vulkan/Shader.h"
#include "graphics/vulkan/VulkanHandler.h"
#include "resources/ResourceManager.h"

Skybox::Skybox(std::vector<std::string> cubemapFaces) :
    _pipelineId {GraphicsPipelineId(VK_PRIMITIVE_TOPOLOGY_LINE_LIST, VK_POLYGON_MODE_FILL,
      MESH_LAYOUT_UNDEFINED, ENGINE_PIPELINE_ID_SKYBOX)} {
  _mesh = ResourceManager::generateCube();
  _cubemap = ResourceManager::loadImage(cubemapFaces);

  std::vector<Shader> shaders {};
  shaders.emplace_back(ResourceManager::assetsPath + "/shaders/skybox_vert.spv",
    VK_SHADER_STAGE_VERTEX_BIT);
  shaders.emplace_back(ResourceManager::assetsPath + "/shaders/skybox_frag.spv",
    VK_SHADER_STAGE_FRAGMENT_BIT);

  std::vector<VkPushConstantRange> ranges = {VulkanHandler::getDefaultPushConstantRange()};

  std::vector<DescriptorSetLayoutType> layoutTypes = {DESCRIPTOR_SET_LAYOUT_GLOBAL, DESCRIPTOR_SET_LAYOUT_SKYBOX};

  VulkanHandler::loadPipeline(_pipelineId, shaders, ranges, layoutTypes);
}

std::shared_ptr<Mesh> Skybox::getMesh() const { return _mesh; }

std::shared_ptr<Image> Skybox::getCubeMap() const { return _cubemap; }

GraphicsPipelineId Skybox::getGraphicsPipelineId() const { return _pipelineId; }
