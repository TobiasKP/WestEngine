#pragma once

#include <cstdint>

class IComponentArray
{
public:
  virtual ~IComponentArray()            = default;
  virtual bool remove(std::uint32_t id) = 0;
};
