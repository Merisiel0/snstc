#pragma once

class GraphicsPipeline;
class Image;

class Material {
  GraphicsPipeline* pipeline;
  Image* albedo;
  Image* normal;
  Image* roughness;
  Image* metalness;
  //Image* specular;
  Image* height;
  //Image* opacity;
  //Image* ambiantOcclusion;
  //Image* emissive;

  Material()
};