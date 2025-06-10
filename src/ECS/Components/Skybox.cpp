#include "Skybox.h"

#include "graphics/vulkan/DescriptorManager.h"
#include "graphics/vulkan/GraphicsPipeline.h"
#include "graphics/vulkan/Image.h"
#include "graphics/vulkan/PipelineLayout.h"
#include "graphics/vulkan/Shader.h"
#include "graphics/vulkan/VulkanHandler.h"
#include "resources/Mesh.h"

Skybox::Skybox(std::vector<std::string> cubemapFaces) : _mesh {Mesh::generateCube()} {
  _cubemap = Image::load(cubemapFaces);

  for(int i = 0; i < Swapchain::FRAME_OVERLAP; i++) {
    _descriptorSets[i] = DescriptorManager::allocateSet(DESCRIPTOR_SET_LAYOUT_SKYBOX);
  }

  std::vector<std::shared_ptr<Shader>> shaders {};
  shaders.push_back(
    Shader::load(assetsPath + "/shaders/skybox_vert.spv", VK_SHADER_STAGE_VERTEX_BIT));
  shaders.push_back(
    Shader::load(assetsPath + "/shaders/skybox_frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT));

  std::vector<VkPushConstantRange> ranges = {VulkanHandler::getDefaultPushConstantRange()};

  std::vector<DescriptorSetLayoutType> layoutTypes = {DESCRIPTOR_SET_LAYOUT_GLOBAL,
    DESCRIPTOR_SET_LAYOUT_SKYBOX};

  std::shared_ptr<PipelineLayout> layout = PipelineLayout::load(layoutTypes, ranges);

  GraphicsPipelineSettings pipelineSettings = GraphicsPipelineSettings(layout, shaders,
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL, true);

  _pipeline = GraphicsPipeline::load(pipelineSettings);
}

std::shared_ptr<Mesh> Skybox::getMesh() const { return _mesh; }

std::shared_ptr<Image> Skybox::getCubeMap() const { return _cubemap; }

const GraphicsPipeline& Skybox::getPipeline() const { return *_pipeline; }
