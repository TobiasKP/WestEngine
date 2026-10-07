#pragma once

#include "../Components/Movement.hpp"
#include "Entity.h"

#include <functional>
#include <optional>
class World : public Entity
{
public:
  World();
  ~World();

  std::int32_t worldPosToTile(double x, double y);
  std::optional<glm::vec3> tileToWorldPos(std::int32_t idx);
  std::int32_t calculateIndex(double x, double y);
  std::vector<std::int32_t> getReachableTiles(std::int32_t row,
                                              std::int32_t col,
                                              std::int32_t range,
                                              algorithm a,
                                              void* callee,
                                              const std::function<bool(std::uint32_t)>& isEnemy = {});
  std::vector<std::int32_t> getPath(std::int32_t from,
                                    std::int32_t to,
                                    std::int32_t range,
                                    const std::function<bool(std::uint32_t)>& isEnemy = {});
  std::vector<std::int32_t>
  getEntitiesInRange(std::int32_t row, std::int32_t col, std::int32_t range, algorithm a, std::int32_t me);
  void addEntityIdToIdx(float x, float y, std::uint32_t id);
  void updateEntityIdToIdx(float x, float y, std::uint32_t id);
  void removeEntityFromGrid(std::uint32_t id);
  std::uint32_t getEntityByIdx(std::int32_t idx);
  void clearFlag(std::uint32_t flag);
  void setFlag(std::uint32_t flag, std::int32_t idx);
  void updateVisibility(const std::vector<std::pair<std::int32_t, std::int32_t>>& origins);
  bool isVisible(std::int32_t idx) const;

  inline void resetDirty()
  {
    _dirty.store(false);
  }

  inline void setSkybox(const std::string& guid)
  {
    _skybox = guid;
  }

  inline void setSkyboxShader(std::uint32_t shaderId)
  {
    _skyboxShaderId = shaderId;
  }

  inline std::string& getSkybox()
  {
    return _skybox;
  }

  inline std::int32_t getGridSize()
  {
    return _dimension;
  }

  inline bool isDirty()
  {
    return _dirty;
  }

  inline glm::vec2& getOrigin()
  {
    return _origin;
  }

  inline std::uint32_t getSkyboxShader()
  {
    return _skyboxShaderId;
  }

  std::vector<std::uint32_t>& getFlagData()
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

private:
  std::vector<std::int32_t> getTilesByAlgorithm(std::int32_t row, std::int32_t col, std::int32_t range, algorithm a);
  std::vector<std::int32_t>
  floodFill(std::int32_t start, std::int32_t range, const std::function<bool(std::uint32_t)>& isEnemy);
  bool canEndOn(const std::vector<std::int32_t>& parents, std::int32_t start, std::int32_t idx);

  std::mutex _mutex;
  std::uint32_t _dimension, _tileSize, _skyboxShaderId;
  std::int32_t _lastIdx;
  glm::vec2 _origin;
  std::vector<std::uint32_t> _vflags;
  std::unordered_map<std::int32_t, std::uint32_t> _idxToEntityId;
  std::unordered_map<std::uint32_t, std::int32_t> _entityIdToIdx;
  std::atomic<bool> _dirty;
  std::string _skybox;
};
