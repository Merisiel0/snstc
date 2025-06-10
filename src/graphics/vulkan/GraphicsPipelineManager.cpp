#include "GraphicsPipelineManager.h"

#include "GraphicsPipeline.h"
#include "PipelineLayout.h"

std::shared_ptr<GraphicsPipeline> GraphicsPipelineManager::createPipeline(
  std::vector<std::shared_ptr<Shader>> shaders,
  std::vector<DescriptorSetLayoutType> setLayouts,
  std::vector<VkPushConstantRange> pushConstantRanges,
  VkPrimitiveTopology primitiveTopology,
  VkPolygonMode polygonMode,
  bool depthWrite) {
  // return pipeline if it already exists
  GraphicsPipelineSettings pipelineSettings = GraphicsPipelineSettings();
  const auto& it0 = std::find_if(_pipelines.begin(), _pipelines.end(),
    [pipelineSettings](std::weak_ptr<GraphicsPipeline> pipeline) {
      if(pipeline.expired()) return false;

      std::shared_ptr<GraphicsPipeline> pipelineSPtr;

      return pipelineSPtr->matchSettings(pipelineSettings);
    });

  if(it0 != _pipelines.end()) { return it0->lock(); }

  // reuse layout if it already exists
  std::shared_ptr<PipelineLayout> layout = nullptr;
  PipelineLayoutSettings layoutSettings = PipelineLayoutSettings(setLayouts, pushConstantRanges);
  const auto& it1 = std::find_if(_layouts.begin(), _layouts.end(),
    [layoutSettings](std::weak_ptr<PipelineLayout> layout) {
      if(layout.expired()) return false;

      std::shared_ptr<PipelineLayout> layoutSPtr = layout.lock();

      return layoutSPtr->matchSettings(layoutSettings);
    });

  // else create layout
  if(it1 != _layouts.end()) {
    layout = (*it1).lock();
  } else {
    layout = std::make_shared<PipelineLayout>(setLayouts, pushConstantRanges);
    _layouts.push_back(layout);
  }

  // create pipeline
  std::shared_ptr<GraphicsPipeline> pipeline = std::make_shared<GraphicsPipeline>(*layout, shaders,
    primitiveTopology, polygonMode, depthWrite);

  return pipeline;
}

std::vector<std::shared_ptr<PipelineLayout>> GraphicsPipelineManager::getLayoutsWithSet(
  DescriptorSetLayoutType setLayout) {
  std::vector<std::shared_ptr<PipelineLayout>> result {};
  for(const auto& layout : _layouts) {
    if(!layout.expired()) {
      auto sptr = layout.lock();
      if(sptr->hasSetLayout(setLayout)) { result.push_back(sptr); }
    }
  }

  return result;
}

void GraphicsPipelineManager::cleanupExpiredLayouts() {
  for(auto it = _layouts.begin(); it != _layouts.end(); it++) {
    if(it->expired()) { _layouts.erase(it); }
  }
}

void GraphicsPipelineManager::cleanupExpired() {
  for(auto it = _pipelines.begin(); it != _pipelines.end(); it++) {
    if(it->expired()) { _pipelines.erase(it); }
  }
}

void GraphicsPipelineManager::cleanup() {
  _pipelines.clear();
  _layouts.clear();
}