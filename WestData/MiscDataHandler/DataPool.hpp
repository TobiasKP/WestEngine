#pragma once

#include "../Constants/Limits.hpp"
#include "../Data/Model.hpp"

#include <array>
#include <unordered_map>
#include <WestLogger.h>

class DataPool
{
public:
  DataPool(WestLogger* l);
  ~DataPool();

  void init();
  void shutdown();
  const Model* getNextFreeProjectile(const std::string& type);

  std::array<Model, Limit::cachesize>& getSceneModels();
  bool addModelToScene(Model&& m);
  bool deleteModelFromScene(const std::string& guid);
  const Model* getModelByGuid(const std::string& guid);
  const Model* getModelByName(const std::string& name);

private:
  WestLogger* _logger;
  std::uint32_t _currentIdx;
  std::array<Model, Limit::projectilePoolsize> _projectiles;
  std::array<Model, Limit::cachesize> _sceneModels;
  std::unordered_map<std::string, std::uint32_t> _guidToIndex;
  std::unordered_map<std::string, std::string> _nameToGuid;
};
