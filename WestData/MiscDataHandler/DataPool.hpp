#pragma once

#include "../Constants/Limits.hpp"
#include "../Data/Model.hpp"

#include <array>
#include <unordered_map>

class DataPool
{
public:
  DataPool();
  ~DataPool();

  void init();
  void shutdown();
  const Model& getNextFree(const std::string& type);
  const Model& getModelByGuid(const std::string& guid);
  std::array<Model, Limit::cachesize>& getSceneModels();

private:
  std::array<Model, Limit::projectilePoolsize> _projectiles;
  std::array<Model, Limit::cachesize> _sceneModels;
  std::unordered_map<std::string, std::uint32_t> _guidToIndex;
};
