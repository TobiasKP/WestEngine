#include "../WestAssetFacade.hpp"

using namespace WestData;

WestAssetFacade& WestAssetFacade::getAssetFacade()
{
  static WestAssetFacade instance;
  return instance;
};


WestAssetFacade::WestAssetFacade()
{
  _logger.log(Level::Info, "|*| Starting up AssetFacade");
  _controller = std::make_unique<ResourceController>(&_logger);
  _controller->init();
};

WestAssetFacade::~WestAssetFacade()
{
  _controller->shutdown();
};

const std::array<Model, Limit::cachesize>& WestAssetFacade::getSceneModels()
{
  return _controller->getSceneModels();
};

const Model* WestAssetFacade::requestModelFromScene(const std::string& uuid)
{
  return _controller->getModel(uuid);
};

const bool WestAssetFacade::deleteModelFromScene(const std::string& uuid)
{
  return _controller->deleteModel(uuid);
};

const std::string WestAssetFacade::addModelToScene(const std::string& path)
{
  return _controller->addModel(path);
};
