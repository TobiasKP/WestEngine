#include "../../CoreHeaders/Entity/World.hpp"

#include "../../Constants/Systems.hpp"
#include "../../CoreHeaders/Interfaces/ISystem.h"

#include <deque>

World::World()
{
  std::lock_guard<std::mutex> lock(_mutex);
  _lastIdx = -1;
  _dirty   = true;
  _skybox  = "";
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

void World::updateVisibility(const std::vector<std::pair<std::int32_t, std::int32_t>>& origins)
{
  clearFlag(0x0004u);
  for (const auto& [tile, range] : origins)
  {
    if (tile < 0)
    {
      continue;
    }
    std::int32_t row = tile / _dimension;
    std::int32_t col = tile % _dimension;
    for (std::int32_t idx : getTilesByAlgorithm(row, col, range, algorithm::MANHATTAN))
    {
      setFlag(0x0004u, idx);
    }
  }
}

bool World::isVisible(std::int32_t idx) const
{
  if (idx < 0 || idx >= static_cast<std::int32_t>(_vflags.size()))
  {
    return false;
  }
  return (_vflags[idx] & 0x0004u) != 0;
}

void World::addEntityIdToIdx(float x, float y, std::uint32_t id)
{
  std::int32_t tile = calculateIndex(x, y);
  if (tile != -1)
  {
    _idxToEntityId[tile] = id;
    _entityIdToIdx[id]   = tile;
  }
}

void World::updateEntityIdToIdx(float x, float y, std::uint32_t id)
{
  if (!_entityIdToIdx.contains(id))
  {
    return;
  }
  removeEntityFromGrid(id);
  addEntityIdToIdx(x, y, id);
}

std::uint32_t World::getEntityByIdx(std::int32_t idx)
{
  if (_idxToEntityId.contains(idx))
  {
    return _idxToEntityId[idx];
  }
  return 0;
}

void World::removeEntityFromGrid(std::uint32_t id)
{
  if (_entityIdToIdx.contains(id))
  {
    std::int32_t idx = _entityIdToIdx[id];
    _entityIdToIdx.erase(id);
    _idxToEntityId.erase(idx);
  }
}


std::vector<std::int32_t>
World::getReachableTiles(std::int32_t row,
                         std::int32_t col,
                         std::int32_t range,
                         algorithm a,
                         void* callee,
                         const std::function<bool(std::uint32_t)>& isEnemy)
{
  std::int32_t start                = row * _dimension + col;
  std::vector<std::int32_t> parents = floodFill(start, range, isEnemy);
  std::vector<std::int32_t> result  = getTilesByAlgorithm(row, col, range, a);
  std::erase_if(result, [&](std::int32_t idx) { return !canEndOn(parents, start, idx); });
  ISystem* c = (ISystem*)callee;
  if (c->getName() != Systems::PLAYER_CONTROL)
  {
    return result;
  }
  for (std::int32_t idx : result)
  {
    setFlag(0x0002u, idx);
  }
  return result;
}

std::vector<std::int32_t>
World::getPath(std::int32_t from, std::int32_t to, std::int32_t range, const std::function<bool(std::uint32_t)>& isEnemy)
{
  std::vector<std::int32_t> path;
  std::vector<std::int32_t> parents = floodFill(from, range, isEnemy);
  if (to == from || !canEndOn(parents, from, to))
  {
    return path;
  }
  for (std::int32_t idx = to; idx != from; idx = parents[idx])
  {
    path.push_back(idx);
  }
  std::reverse(path.begin(), path.end());
  return path;
}

std::vector<std::int32_t>
World::floodFill(std::int32_t start, std::int32_t range, const std::function<bool(std::uint32_t)>& isEnemy)
{
  std::int32_t dimension = static_cast<std::int32_t>(_dimension);
  std::vector<std::int32_t> parents(_vflags.size(), -1);
  if (start < 0 || start >= static_cast<std::int32_t>(_vflags.size()))
  {
    return parents;
  }
  std::vector<std::int32_t> depth(_vflags.size(), 0);
  std::deque<std::int32_t> open{start};
  parents[start] = start;
  while (!open.empty())
  {
    std::int32_t idx = open.front();
    open.pop_front();
    if (depth[idx] >= range)
    {
      continue;
    }
    std::int32_t row = idx / dimension;
    std::int32_t col = idx % dimension;
    for (auto [dr, dc] : {std::pair{-1, 0}, std::pair{0, -1}, std::pair{0, 1}, std::pair{1, 0}})
    {
      std::int32_t r = row + dr;
      std::int32_t c = col + dc;
      if (r < 0 || r >= dimension || c < 0 || c >= dimension)
      {
        continue;
      }
      std::int32_t next      = r * dimension + c;
      std::uint32_t occupant = getEntityByIdx(next);
      if (parents[next] != -1 || (_vflags[next] & 0x0008u) != 0 || (occupant != 0 && isEnemy && isEnemy(occupant)))
      {
        continue;
      }
      parents[next] = idx;
      depth[next]   = depth[idx] + 1;
      open.push_back(next);
    }
  }
  return parents;
}

bool World::canEndOn(const std::vector<std::int32_t>& parents, std::int32_t start, std::int32_t idx)
{
  return parents[idx] != -1 && (idx == start || getEntityByIdx(idx) == 0);
}

std::vector<std::int32_t>
World::getEntitiesInRange(std::int32_t row, std::int32_t col, std::int32_t range, algorithm a, std::int32_t me)
{
  std::vector<std::int32_t> out;
  std::vector<std::int32_t> result = getTilesByAlgorithm(row, col, range, a);
  for (std::int32_t idx : result)
  {
    if (_idxToEntityId.contains(idx) && me != _idxToEntityId[idx])
    {
      out.push_back(idx);
    }
  }

  return out;
}

std::vector<std::int32_t>
World::getTilesByAlgorithm(std::int32_t row, std::int32_t col, std::int32_t range, algorithm a)
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
        }
      }
    }
  }
  return result;
}
