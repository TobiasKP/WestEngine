#pragma once

#include "../AssetPipeline/Converter.hpp"
#include "../Data/Model.hpp"
#include "../MiscDataHandler/DataPool.hpp"
#include "ModelBuilder.hpp"

#include <memory>
#include <WestLogger.h>

class ResourceHandler
{
public:
  ResourceHandler(WestLogger* l);
  ~ResourceHandler();

  void init();
  void shutdown();

private:
  WestLogger* _logger;
  std::unique_ptr<ModelBuilder> _builder;
  std::unique_ptr<DataPool> _pool;
  std::unique_ptr<Converter> _converter;

  std::vector<Model> _sceneModels;
};
