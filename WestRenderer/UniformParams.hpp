#pragma once

#include <cstdint>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <vector>

struct UniformParams
{
  std::vector<std::uint32_t> flags;
};

struct WorldUniformParams : UniformParams
{
  std::int32_t worldDimension;
  glm::vec2 worldOrigin;
  std::string& skyboxGuid; 
  GLuint skyboxShaderId;
};

struct EntityUniformParams : UniformParams
{
  std::unordered_map<std::string, glm::vec3> diffuseColor, emissiveColor;
  std::int32_t texture;
  glm::mat4 transform;
};
