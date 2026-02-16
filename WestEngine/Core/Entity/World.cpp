#include "../../CoreHeaders/Entity/World.hpp"


World::World()
{
  std::lock_guard<std::mutex> lock(_mutex);
  _lastIdx = -1;
}

World::~World()
{
  _vflags.clear();
}

std::int32_t World::worldPosToTile(double x, double y)
{
  std::int32_t index = calculateIndex(x, y);
  if (index == _lastIdx)
  {
    return index;
  }
  {
    std::lock_guard<std::mutex> lock(_mutex);
    for (std::int32_t i = 0; i < _vflags.size(); i++)
    {
      _vflags[i] &= ~0x0001u;
    }
    if (index >= 0)
    {
      _vflags[index] |= 0x0001u;
    }
    _lastIdx = index;
    return index;
  }
}

std::optional<glm::vec3> World::tileToWorldPos(std::int32_t idx)
{
  if(idx == -1) {
    return {};
  }
  int column = idx % _dimension;
  int row    = idx / _dimension;
  return glm::vec3(column + 0.5, 0, row + 0.5);
}

std::int32_t World::calculateIndex(double x, double y)
{
  if (x < 0 || x >= _dimension || y < 0 || y >= _dimension)
  {
    return -1;
  }
  else
  {
    return std::int32_t(std::floor(x) + std::floor(y) * _dimension);
  }
}
