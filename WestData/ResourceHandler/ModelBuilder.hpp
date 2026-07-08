#pragma once

#include "../AssetPipeline/BinaryLoader.hpp"
#include "../Data/Model.hpp"

class ModelBuilder
{
public:
  ModelBuilder(WestLogger* l);
  ~ModelBuilder();

  std::optional<Model> createModel(const std::string& path);
  bool deleteModel(const std::string& hash);
  void init();
  void shutdown();

private:
  std::unique_ptr<BinaryLoader> _loader;
  WestLogger* _logger;
};
