#pragma once

#include <atomic>
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
  GLint diffuseColorUniform = -1;
  glm::vec3 ambientColor;   // Ka
  glm::vec3 specularColor;  // Ks
  glm::vec3 emissiveColor;  // Ke
  float specularExponent;   // Ns
  float opacity;            // d
  float refractiveIndex;    // Ni
  Texture* diffuseTexture;
  std::atomic<bool> dirty{true};

  Material() = default;
  Material(Material&& o) noexcept
    : id(o.id), name(std::move(o.name)), diffuseColor(o.diffuseColor), diffuseColorUniform(o.diffuseColorUniform),
      ambientColor(o.ambientColor), specularColor(o.specularColor), emissiveColor(o.emissiveColor),
      specularExponent(o.specularExponent), opacity(o.opacity), refractiveIndex(o.refractiveIndex),
      diffuseTexture(o.diffuseTexture), dirty(o.dirty.load())
  {
    o.diffuseTexture = nullptr;
  }
  Material& operator=(Material&& o) noexcept
  {
    id                  = o.id;
    name                = std::move(o.name);
    diffuseColor        = o.diffuseColor;
    diffuseColorUniform = o.diffuseColorUniform;
    ambientColor        = o.ambientColor;
    specularColor       = o.specularColor;
    emissiveColor       = o.emissiveColor;
    specularExponent    = o.specularExponent;
    opacity             = o.opacity;
    refractiveIndex     = o.refractiveIndex;
    diffuseTexture      = o.diffuseTexture;
    dirty.store(o.dirty.load());
    o.diffuseTexture = nullptr;
    return *this;
  }
};
