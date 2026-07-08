#include "ResourceController.hpp"

ResourceController::ResourceController(WestLogger* l)
{
  _logger  = l;
  _builder = std::make_unique<ModelBuilder>(l);
  _pool    = std::make_unique<DataPool>();
}

ResourceController::~ResourceController() {}

void ResourceController::init()
{
  _builder->init();
  _pool->init();
}

void ResourceController::shutdown()
{
  _builder->shutdown();
  _pool->shutdown();
};

const std::array<Model, Limit::cachesize>& ResourceController::getSceneModels() const
{
  return _pool->getSceneModels();
};

std::string ResourceController::addModel(const std::string& path)
{
  std::optional<Model> m = _builder->createModel(path);

  if (!m.has_value())
  {
    // TODO: Error handling
    return "";
  }
  std::string guid = m.value().getGuid();
  bool res         = _pool->addModelToScene(std::move(m.value()));
  if (!res)
  {
    return "";
  }
  return guid;
}

const Model* ResourceController::getModel(const std::string& guid)
{
  const Model* m = _pool->getModelByGuid(guid);
  if (m != nullptr)
  {
    return m;
  }
  // TODO: Error handling;
  return {};
}

bool ResourceController::deleteModel(const std::string& guid)
{
  return _pool->deleteModelFromScene(guid);
}
