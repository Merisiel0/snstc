#include "RayTracingPipeline.h"

#include "Device.h"
#include "Shader.h"

RayTracingPipeline::RayTracingPipeline(Device* device, Shader* rayGen, Shader* miss, Shader* closestHit) : IPipeline(device) {
  std::vector<VkPipelineShaderStageCreateInfo> stageCreateInfos = {
    rayGen->getStageCreateInfo(), miss->getStageCreateInfo(), closestHit->getStageCreateInfo()
  };

  std::vector<VkRayTracingShaderGroupCreateInfoKHR> shaderGroups;

  VkRayTracingShaderGroupCreateInfoKHR group{};
  group.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
  //group.pNext;
  //group.type;
  group.generalShader = VK_SHADER_UNUSED_KHR;
  group.closestHitShader = VK_SHADER_UNUSED_KHR;
  group.anyHitShader = VK_SHADER_UNUSED_KHR;
  group.intersectionShader = VK_SHADER_UNUSED_KHR;
  //group.pShaderGroupCaptureReplayHandle;

  group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
  group.generalShader = 0;
  shaderGroups.push_back(group);

  group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
  group.generalShader = 1;
  shaderGroups.push_back(group);

  group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
  group.generalShader = VK_SHADER_UNUSED_KHR;
  group.closestHitShader = 2;
  shaderGroups.push_back(group);

  VkPipelineLayoutCreateInfo layoutCreateInfo{};
  layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  //layoutCreateInfo.pNext;
  //layoutCreateInfo.flags;
  //layoutCreateInfo.setLayoutCount;
  //layoutCreateInfo.pSetLayouts;
  //layoutCreateInfo.pushConstantRangeCount;
  //layoutCreateInfo.pPushConstantRanges;

  VK_CHECK(vkCreatePipelineLayout(device->handle, &layoutCreateInfo, nullptr, &_layout));

  VkRayTracingPipelineCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
  //createInfo.pNext;
  createInfo.flags = VK_PIPELINE_CREATE_RAY_TRACING_NO_NULL_CLOSEST_HIT_SHADERS_BIT_KHR | VK_PIPELINE_CREATE_RAY_TRACING_NO_NULL_MISS_SHADERS_BIT_KHR;
  createInfo.stageCount = static_cast<uint32_t>(stageCreateInfos.size());
  createInfo.pStages = stageCreateInfos.data();
  createInfo.groupCount = static_cast<uint32_t>(shaderGroups.size());
  createInfo.pGroups = shaderGroups.data();
  createInfo.maxPipelineRayRecursionDepth = 1;
  //createInfo.pLibraryInfo;
  //createInfo.pLibraryInterface;
  //createInfo.pDynamicState;
  createInfo.layout = _layout;
  //createInfo.basePipelineHandle;
  //createInfo.basePipelineIndex;

  vkCreateRayTracingPipelinesKHR(device->handle, VK_NULL_HANDLE, VK_NULL_HANDLE, 1, &createInfo, nullptr, &handle);
}
