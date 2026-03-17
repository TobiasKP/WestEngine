#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

struct Texture
{
  std::int32_t id = -1;
  GLuint uniform  = -1;
};

struct Material
{
  std::int32_t id;

  std::string name;
  glm::vec3 diffuseColor;  // Kd - base color
  GLint diffuseColorUniform = -1, emissiveColorUniform = -1;
  glm::vec3 ambientColor;                  // Ka
  glm::vec3 specularColor;                 // Ks
  glm::vec3 emissiveColor = glm::vec3(0);  // Ke
  float specularExponent;                  // Ns
  float opacity;                           // d
  float refractiveIndex;                   // Ni
  Texture* diffuseTexture;
};
