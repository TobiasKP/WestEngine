#pragma once


#include <cstdint>
#include <glm/glm.hpp>
#include <vector>


struct UniformParams
{
  std::vector<std::uint32_t> flags;
};

struct WorldUniformParams : UniformParams
{
  std::int32_t worldDimension;
  glm::vec2 worldOrigin;
};

struct EntityUniformParams : UniformParams
{
  glm::vec3 diffuseColor, emissiveColor;
  std::int32_t texture;
  glm::mat4 transform;
};
