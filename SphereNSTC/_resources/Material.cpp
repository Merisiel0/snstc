#include "Material.h"

#include "_graphics/Image.h"

Material::Material(GraphicsPipeline* pipeline,
  const char* albedoMapPass,
  const char* normalMapPass,
  const char* rougnessMapPass,
  const char* metalnessMapPass,
  const char* heightMapPass) {

  this->pipeline = pipeline;

  if(albedoMapPass) albedo = new Image(albedoMapPass);
  if(normalMapPass) normal = new Image(normalMapPass);
  if(rougnessMapPass) roughness = new Image(rougnessMapPass);
  if(metalnessMapPass) metalness = new Image(metalnessMapPass);
  if(heightMapPass) height = new Image(heightMapPass);
}