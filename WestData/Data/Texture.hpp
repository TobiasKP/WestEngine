#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct Texture
{
  std::vector<unsigned char> imageData;
  std::int32_t width, height, numComponents;
  std::string type;
};
