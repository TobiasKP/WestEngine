#pragma once

#include "../Interfaces/IComponent.h"

#include <cstdint>
#include <PoolAllocator.h>
#include <GL/glew.h>

struct Model : public IComponent
{
  std::int32_t id;
  std::int32_t vertexCount; 

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
  GLint debugColorUniform;


private:
  static inline PoolAllocator* _allocator = new PoolAllocator();
};
