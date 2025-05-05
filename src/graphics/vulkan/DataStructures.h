#pragma once

#include "vulkan/vulkan.h"

/// @brief Container for `VkPipelineColorBlendStateCreateInfo` and all its necessary data.
struct PipelineColorBlendStateCreateInfoData {
  std::vector<VkPipelineColorBlendAttachmentState> attachments;

  VkPipelineColorBlendStateCreateInfo info;
};

/// @brief Container for `VkPipelineDynamicStateCreateInfo` and all its necessary data.
struct PipelineDynamicStateCreateInfoData {
  std::vector<VkDynamicState> states;

  VkPipelineDynamicStateCreateInfo info;
};

/// @brief Container for `VkGraphicsPipelineCreateInfo` and all its necessary data.
struct GraphicsPipelineCreateInfoData {
  VkPipelineRenderingCreateInfo rendering;
  VkPipelineVertexInputStateCreateInfo vertexInput;
  VkPipelineInputAssemblyStateCreateInfo inputAssembly;
  VkPipelineTessellationStateCreateInfo tessellation;
  VkPipelineViewportStateCreateInfo viewport;
  VkPipelineRasterizationStateCreateInfo rasterization;
  VkPipelineMultisampleStateCreateInfo multisample;
  VkPipelineDepthStencilStateCreateInfo depthStencil;
  PipelineColorBlendStateCreateInfoData colorBlend;
  PipelineDynamicStateCreateInfoData dynamicState;

  VkGraphicsPipelineCreateInfo info;
};

/// @brief Container for `VkRenderingInfo` and all its necessary data.
struct RenderingInfoData {
  std::vector<VkRenderingAttachmentInfo> colorAttachments;
  VkRenderingAttachmentInfo depthAttachment;
  VkRenderingAttachmentInfo stencilAttachment;

  VkRenderingInfo info;
};

/// @brief Container for `VkSubmitInfo2` and all its necessary data.
struct SubmitInfo2Data {
  std::vector<VkSemaphoreSubmitInfo> waitSemaphoreInfos;
  std::vector<VkCommandBufferSubmitInfo> commandBufferSubmitInfos;
  std::vector<VkSemaphoreSubmitInfo> signalSemaphoreInfos;

  VkSubmitInfo2 info;
};

/// @brief Container for `VkWriteDescriptorSet` and all its necessary data.
struct WriteDescriptorSetData {
  VkDescriptorImageInfo imageInfo;
  VkDescriptorBufferInfo bufferInfo;
  VkBufferView texelBufferView;

  VkWriteDescriptorSet info;
};

struct DescriptorSetAllocateInfoData {
  std::vector<VkDescriptorSetLayout> setLayouts;
  VkDescriptorSetAllocateInfo info;
};