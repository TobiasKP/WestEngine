#pragma once

#include "../Components/Movement.hpp"
#include "Entity.h"

class World : public Entity
{
public:
  World();
  ~World();

  std::int32_t worldPosToTile(double x, double y);
  std::optional<glm::vec3> tileToWorldPos(std::int32_t idx);
  std::int32_t calculateIndex(double x, double y);
  std::vector<std::int32_t>
  getReachableTiles(std::int32_t row, std::int32_t col, std::int32_t range, algorithm a, void* callee);
  void clearFlag(std::uint32_t flag);
  void setFlag(std::uint32_t flag, std::int32_t idx);

  inline GLuint getFlagUniform()
  {
    return _utileFlags;
  }
  inline GLuint getGridUniform()
  {
    return _ugridSize;
  }
  inline std::int32_t getGridSize()
  {
    return _dimension;
  }
  std::vector<std::uint32_t> getFlagData()
  {
    std::lock_guard<std::mutex> lock(_mutex);
    return _vflags;
  }

  void setCreationInformation(std::uint32_t d, std::uint32_t s, glm::vec2 o)
  {
    _dimension = d;
    _tileSize  = s;
    _origin    = o;
    _vflags.resize(_dimension * _dimension);
    std::fill(_vflags.begin(), _vflags.end(), 0);
  };
  inline void setFlagUniform(GLuint u)
  {
    _utileFlags = u;
  }
  inline void setGridUniform(GLuint u)
  {
    _ugridSize = u;
  }


private:
  std::mutex _mutex;
  std::uint32_t _dimension, _tileSize;
  std::int32_t _lastIdx;
  glm::vec2 _origin;
  std::vector<std::uint32_t> _vflags;
  GLuint _ugridSize, _utileFlags;
};
