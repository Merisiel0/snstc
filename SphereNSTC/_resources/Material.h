#pragma once

class GraphicsPipeline;
class Image;

class Material {
  GraphicsPipeline* pipeline;
  Image* albedo{ nullptr };
  Image* normal{ nullptr };
  Image* roughness{ nullptr };
  Image* metalness{ nullptr };
  //Image* specular{ nullptr };
  Image* height{ nullptr };
  //Image* opacity{ nullptr };
  //Image* ambiantOcclusion{ nullptr };
  //Image* emissive{ nullptr };

  Material(GraphicsPipeline* pipeline,
    const char* albedoMapPath,
    const char* normalMapPath,
    const char* rougnessMapPath,
    const char* metalnessMapPath,
    const char* heightMapPath);
};