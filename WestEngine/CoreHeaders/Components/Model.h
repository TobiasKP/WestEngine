#pragma once

#include <cstdint>
#include <GL/glew.h>

struct Model
{
  std::int32_t id;
  std::int32_t vertexCount;
  GLint debugColorUniform;
};
