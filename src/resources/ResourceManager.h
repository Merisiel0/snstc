#pragma once

#include "../graphics/vulkan/Image.h"
#include "Material.h"
#include "Mesh.h"

#include <memory>
#include <type_traits>
#include <unordered_map>
#include <vector>

class ResourceManager {
private:
  /// @brief In order to have unique ids, this prefix is put in front of every dynamically generated resources.
  ///
  /// This means no user provided resource should start with this prefix.
  static constexpr const char* _generationPrefix = "dgyenn";

  static std::unordered_map<std::string, std::weak_ptr<Image>> _images;
  static std::unordered_map<std::string, std::weak_ptr<Material>> _materials;
  static std::unordered_map<std::string, std::weak_ptr<Mesh>> _meshes;

public:
  static std::string assetsPath;

  /// @brief Converts a partial path (from the source directory) to a complete path (from root).
  /// @param partialPath a path to convert.
  /// @return the complete path: ${sourceDir} + partialPath.
  static std::string getCompletePath(std::string partialPath);

  /// @brief Cleans up pointers to expired resources.
  static void cleanupResources();

  /// @brief Loads an image from the provided path.
  /// @param path path to the image.
  /// @return an image if the path exist, nullptr otherwise.
  static std::shared_ptr<Image> loadImage(std::string path);

  /// @brief Loads an image of the provided dimensions and color.
  /// @param width the width of the image.
  /// @param height the height of the image.
  /// @param color the color of the image.
  /// @return an image.
  static std::shared_ptr<Image> loadImage(int width, int height, Color color);

  /// @brief Loads a material from the provided path.
  /// @param path path to the material.
  /// @return a PBR material the path exists, nullptr otherwise.
  static std::shared_ptr<Material> loadMaterial(std::string path = "");

  /// @brief Loads a color material of the provided color.
  /// @param color a color.
  /// @return a PBR material with a single albedo map.
  static std::shared_ptr<Material> loadMaterial(Color color);

  /// @brief Loads a mesh from the provide path.
  /// @param path path to the mesh.
  /// @return a mesh.
  static std::shared_ptr<Mesh> loadMesh(std::string path);

  /// @brief Generates a cube mesh.
  /// @param color a color.
  /// @return a shared pointer to a cube mesh.
  static std::shared_ptr<Mesh> generateCube(Color color = {0, 0, 0, 1});

  /// @brief Generates a plane mesh.
  /// @param dimensions extent of the plane in x and y.
  /// @param vertexAmounts amount of vertices in x and y. Minimum of {2, 2}.
  /// @param color a color.
  /// @return a shared pointer to a plane mesh.
  static std::shared_ptr<Mesh> generatePlane(vec2 dimensions, vec2 vertexAmounts,
    Color color = {0, 0, 0, 1});

  /// @brief Generates a cone mesh.
  /// @param radius radius of the base.
  /// @param height height of the cone.
  /// @param resolution amount of vertices on the circumference.
  /// @param color a color.
  /// @return a shared pointer to a cone mesh.
  static std::shared_ptr<Mesh> generateCone(float radius, float height, int resolution,
    Color color = {0, 0, 0, 1});

  /// @brief Generates a cylinder mesh.
  /// @param radius radius of the base.
  /// @param height height of the cylinder.
  /// @param resolution amount of vertices on the circumference.
  /// @param color a color.
  /// @return a shared pointer to a cylinder mesh.
  static std::shared_ptr<Mesh> generateCylinder(float radius, float height, int resolution,
    Color color = {0, 0, 0, 1});

  /// @brief Generates a UV sphere mesh.
  /// @param nbSlices amount of vertices on the horizontal circumference.
  /// @param nbStacks amount of vertices on the vertical circumference.
  /// @param color a color.
  /// @return a shared pointer to a UV sphere mesh.
  static std::shared_ptr<Mesh> generateUVSphere(int nbSlices, int nbStacks,
    Color color = {0, 0, 0, 1});

  /// @brief Generates an icosphere mesh.
  /// @param nbDivisions how many times it should be divided. 0 is a D20.
  /// @param color a color.
  /// @return a shared pointer to an icosphere mesh.
  static std::shared_ptr<Mesh> generateIcoSphere(int nbDivisions, Color color = {0, 0, 0, 1});
};