#pragma once

#include "../Interfaces/IComponent.h"

#include <cstdint>
#include <GL/glew.h>
#include <PoolAllocator.h>

struct Texture
{
  std::int32_t id = -1;
  GLuint uniform  = -1;
};

struct Model : public IComponent
{
  std::int32_t id;
  std::int32_t vertexCount;
  Texture* texture = nullptr;

  // Overrides
  static void* operator new(size_t size)
  {
    return _allocator->allocate(size);
  }
  static void operator delete(void* ptr, size_t size)
  {
    return _allocator->deallocate(ptr, size);
  }

  // Debug fields
  GLuint debugColorUniform;
  glm::vec3 color;

private:
  static inline PoolAllocator* _allocator = new PoolAllocator();
};
