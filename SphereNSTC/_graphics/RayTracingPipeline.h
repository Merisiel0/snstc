#pragma once

#include "VulkanUtils.h"

#include "Pipeline.h"

class Device;
class Shader;

class RayTracingPipeline : public IPipeline {
public:
  RayTracingPipeline(Device* device, Shader* rayGen, Shader* miss, Shader* closestHit);
};