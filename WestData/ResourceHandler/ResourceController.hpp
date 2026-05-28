#pragma once

#include "../Data/Model.hpp"
#include "../MiscDataHandler/DataPool.hpp"
#include "ModelBuilder.hpp"

#include <memory>
#include <WestLogger.h>

class ResourceController
{
public:
  ResourceController(WestLogger* l);
  ~ResourceController();

  void init();
  void shutdown();
  const std::vector<Model>& getSceneModels();

private:
  WestLogger* _logger;
  std::unique_ptr<ModelBuilder> _builder;
  std::unique_ptr<DataPool> _pool;
};
