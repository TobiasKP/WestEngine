#pragma once

#include <atomic>
#include <GL/glew.h>
#include <glm/glm.hpp>

struct Position
{
  glm::vec3 position = glm::vec3(1.0f), rotation = glm::vec3(1.0f);
  std::int32_t tile = -1;
  float scale       = 1.0f;
  GLuint uniform    = -1;
  std::atomic<bool> dirty{true};

  Position() = default;
  Position(Position&& o) noexcept
    : position(o.position), rotation(o.rotation), tile(o.tile), scale(o.scale), uniform(o.uniform),
      dirty(o.dirty.load())
  {}
  Position& operator=(Position&& o) noexcept
  {
    position = o.position;
    rotation = o.rotation;
    tile     = o.tile;
    scale    = o.scale;
    uniform  = o.uniform;
    dirty.store(o.dirty.load());
    return *this;
  }
};
