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
  const std::array<Model, Limit::cachesize>& getSceneModels() const;
  std::string addModel(const std::string& path);
  std::string addModel(Model& m);
  const Model* getModel(const std::string& guid);
  bool deleteModel(const std::string& guid);
  const Texture loadTexture(const std::string& path);

private:
  WestLogger* _logger;
  std::unique_ptr<ModelBuilder> _builder;
  std::unique_ptr<DataPool> _pool;
  std::shared_ptr<AssetImporter> _importer;
};
