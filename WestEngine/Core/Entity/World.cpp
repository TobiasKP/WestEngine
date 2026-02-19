#include "../../CoreHeaders/Entity/World.hpp"

#include "../../Constants/Systems.hpp"
#include "../../CoreHeaders/Interfaces/ISystem.h"

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
  if (idx == -1)
  {
    return {};
  }
  std::int32_t column = idx % _dimension;
  std::int32_t row    = idx / _dimension;
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

void World::clearFlag(std::uint32_t flag)
{
  for (std::int32_t i = 0; i < _vflags.size(); i++)
  {
    _vflags[i] &= ~flag;
  }
}

void World::setFlag(std::uint32_t flag, std::int32_t idx)
{
  _vflags[idx] |= flag;
}

std::vector<std::int32_t>
World::getReachableTiles(std::int32_t row, std::int32_t col, std::int32_t range, algorithm a, void* callee)
{
  std::vector<std::int32_t> result;
  if (static_cast<std::int32_t>(a) == static_cast<std::int32_t>(algorithm::MANHATTAN))
  {
    for (std::int32_t r = std::max(0, row - range); r <= std::min(_dimension - 1, std::uint32_t(row + range)); r++)
    {
      for (std::int32_t c = std::max(0, col - range); c <= std::min(_dimension - 1, std::uint32_t(col + range)); c++)
      {
        std::int32_t sum = abs(r - row) + abs(c - col);
        if (sum <= range)
        {
          std::int32_t idx = r * _dimension + c;
          result.push_back(idx);
          ISystem* c = (ISystem*)callee;
          if (c->getName() == Systems::PLAYER_CONTROL)
          {
            setFlag(0x0002u, idx);
          }
        }
      }
    }
  }
  return result;
}
