#include "ResourceController.hpp"

#include "../Constants/TextureTypes.hpp"
#include "../Utils/AssetUtils.hpp"

#include <format>

ResourceController::ResourceController(WestLogger* l)
{
  _logger   = l;
  _importer = std::make_shared<AssetImporter>(l);
  _builder  = std::make_unique<ModelBuilder>(l, _importer);
  _pool     = std::make_unique<DataPool>(l);
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

const Texture ResourceController::loadTexture(const std::string& path)
{
  return _importer->textureFromFile(path, TextureTypes::SKYBOX.data());
}

std::string ResourceController::addModel(const std::string& path)
{
  const Model* old = _pool->getModelByName(path);
  if (old != nullptr)
  {
    return old->getGuid();
  }
  std::optional<Model> m = _builder->createModel(path);

  if (!m.has_value())
  {
    // TODO: Error handling
    _logger->log(Level::Error,
                 std::format("|*| Error creating Model for: {} look at trace for more information.\n", path));
    return "";
  }
  std::string guid = m.value().getGuid();
  bool res         = _pool->addModelToScene(std::move(m.value()));
  if (!res)
  {
    _logger->log(Level::Error,
                 std::format("|*| Error adding Model to Scene: {} look at trace for more information.\n", path));
    return "";
  }
  return guid;
}

std::string ResourceController::addModel(Model& m)
{
  std::stringstream ss;
  ss << &m;
  std::string guid = AssetUtils::generateGUID(ss.str());
  m.setGuid(guid);
  bool res = _pool->addModelToScene(std::move(m));
  if (!res)
  {
    _logger->log(Level::Error, std::format("|*| Error adding Model to Scene: Artificially created...\n"));
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
  _logger->log(Level::Error, std::format("|*| Error getting Model: {}, something went wrong.\n", guid));
  // TODO: Error handling;
  return {};
}

bool ResourceController::deleteModel(const std::string& guid)
{
  return _pool->deleteModelFromScene(guid);
}
