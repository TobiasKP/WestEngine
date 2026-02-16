#include "../../CoreHeaders/Entity/World.hpp"

#include <algorithm>
#include <iostream>

World::World()
{
  std::lock_guard<std::mutex> lock(_mutex);
}

World::~World()
{
  _vflags.clear();
}

void World::worldPosToTile(double x, double y)
{
  std::lock_guard<std::mutex> lock(_mutex);
  std::int32_t index =
    std::clamp(std::int32_t(std::floor(x) + std::floor(y) * _dimension), 0, std::int32_t(_vflags.size() - 1));
  for (std::int32_t i = 0; i < _vflags.size(); i++)
  {
    _vflags[i] &= ~0x0001u;
  }
  _vflags[index] |= 0x0001u;
}
