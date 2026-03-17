#include "../../CoreHeaders/Entity/World.hpp"

#include "../../Constants/Systems.hpp"
#include "../../CoreHeaders/Interfaces/ISystem.h"

World::World()
{
  std::lock_guard<std::mutex> lock(_mutex);
  _lastIdx = -1;
  _dirty   = true;
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
    if (_lastIdx >= 0)
    {
      _vflags[_lastIdx] &= ~0x0001u;
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
  return glm::vec3(column + 0.5 + _origin.x, 0, row + 0.5 + _origin.y);
}

std::int32_t World::calculateIndex(double x, double y)
{
  double localX = x - _origin.x;
  double localY = y - _origin.y;
  if (localX < 0 || localX >= _dimension || localY < 0 || localY >= _dimension)
  {
    return -1;
  }
  else
  {
    return std::int32_t(std::floor(localX) + std::floor(localY) * _dimension);
  }
}

void World::clearFlag(std::uint32_t flag)
{
  for (std::int32_t i = 0; i < _vflags.size(); i++)
  {
    _vflags[i] &= ~flag;
  }
  _dirty.store(true, std::memory_order_relaxed);
}

void World::setFlag(std::uint32_t flag, std::int32_t idx)
{
  _vflags[idx] |= flag;
  _dirty.store(true, std::memory_order_relaxed);
}

void World::addEntityIdToIdx(float x, float y, std::uint32_t id)
{
  if (_entityIdToIdx.contains(id))
  {
    std::uint16_t tile = _entityIdToIdx[id];
    _idxToEntityId.erase(tile);
    _entityIdToIdx.erase(id);
  }
  std::int32_t tile = calculateIndex(x, y);
  if (tile != -1)
  {
    _idxToEntityId[tile] = id;
    _entityIdToIdx[id]   = tile;
  }
}

std::uint32_t World::getEntityByIdx(std::int32_t idx)
{
  if (_idxToEntityId.contains(idx))
  {
    return _idxToEntityId[idx];
  }
  return 0;
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
