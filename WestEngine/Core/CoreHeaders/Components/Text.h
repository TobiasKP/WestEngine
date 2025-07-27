#pragma once

#include <GL/glew.h>
#include <cstdint>

#include "../Interfaces/IComponent.h"

struct Text : public IComponent {
  std::uint16_t length; 
  const char* text;
};
