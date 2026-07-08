#include "ModelBuilder.hpp"

ModelBuilder::ModelBuilder(WestLogger* l)
{
  _logger = l;
  _loader = std::make_unique<BinaryLoader>(l);
}

ModelBuilder::~ModelBuilder() {}

void ModelBuilder::init()
{
  _loader->init();
}

void ModelBuilder::shutdown()
{
  _loader->shutdown();
}

std::optional<Model> ModelBuilder::createModel(const std::string& path)
{
  std::optional<Model> result = _loader->load(path);
  if (result.has_value())
  {
    return std::move(*result);
  }
  return {};
}

bool ModelBuilder::deleteModel(const std::string& hash)
{
  return true;
}
