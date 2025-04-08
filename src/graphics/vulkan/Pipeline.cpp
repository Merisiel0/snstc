#include "Pipeline.h"

#include "Device.h"

VkPipelineLayoutCreateInfo IPipeline::getLayoutCreateInfo(
  std::vector<VkPushConstantRange> pushConstantRanges,
  std::vector<VkDescriptorSetLayout> setLayouts) const {
  VkPipelineLayoutCreateInfo info {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  //info.pNext = nullptr;
  //info.flags = 0;

  if(setLayouts.size() > 0) {
    info.setLayoutCount = (uint32_t) setLayouts.size();
    VkDescriptorSetLayout* setLayoutsPtr = new VkDescriptorSetLayout[info.setLayoutCount];
    std::copy(setLayouts.begin(), setLayouts.end(), setLayoutsPtr);
    info.pSetLayouts = setLayoutsPtr;
  }

  if(pushConstantRanges.size() > 0) {
    info.pushConstantRangeCount = (uint32_t) pushConstantRanges.size();
    VkPushConstantRange* rangesPtr = new VkPushConstantRange[pushConstantRanges.size()];
    std::copy(pushConstantRanges.begin(), pushConstantRanges.end(), rangesPtr);
    info.pPushConstantRanges = rangesPtr;
  }

  return info;
}

IPipeline::IPipeline(std::shared_ptr<Device> device,
  std::vector<VkPushConstantRange> pushConstantRanges,
  std::vector<VkDescriptorSetLayout> setLayouts) {
  _device = device;

  VkPipelineLayoutCreateInfo layoutInfo = getLayoutCreateInfo(pushConstantRanges, setLayouts);
  VK_CHECK(vkCreatePipelineLayout(device->handle, &layoutInfo, nullptr, &_layout));
}

IPipeline::~IPipeline() {
  vkDestroyPipeline(_device->handle, handle, nullptr);
  vkDestroyPipelineLayout(_device->handle, _layout, nullptr);
}