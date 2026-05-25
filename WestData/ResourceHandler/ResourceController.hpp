#pragma once

#include "../AssetPipeline/Converter.hpp"
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
  const std::vector<Model>& getScene();

private:
  WestLogger* _logger;
  std::unique_ptr<ModelBuilder> _builder;
  std::unique_ptr<DataPool> _pool;
  std::unique_ptr<Converter> _converter; 

  std::vector<Model> _sceneModels;
};
