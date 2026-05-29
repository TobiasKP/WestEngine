#pragma once

#include "../AssetPipeline/BinaryLoader.hpp"
#include "../Data/Model.hpp"

class ModelBuilder
{
public:
  ModelBuilder();
  ~ModelBuilder();

  const Model& createModel(const std::string path);
  bool deleteModel(const std::string hash);

private:
  std::unique_ptr<BinaryLoader> _loader;
};
