#pragma once

#include "../Interfaces/IComponent.h"

#include <GL/glew.h>
#include <PoolAllocator.h>
#include <string>

struct Texture
{
  std::int32_t id = -1;
  GLuint uniform  = -1;
};

struct Material : public IComponent
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

  static void* operator new(size_t size)
  {
    return _allocator->allocate(size);
  }
  static void operator delete(void* ptr, size_t size)
  {
    return _allocator->deallocate(ptr, size);
  }


private:
  static inline PoolAllocator* _allocator = new PoolAllocator();
};
