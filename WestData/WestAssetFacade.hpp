#pragma once

#include "Data/Model.hpp"
#include "ResourceHandler/ResourceController.hpp"

namespace WestData
{

class WestAssetFacade
{
public:
  static WestAssetFacade& getAssetFacade();

  const std::array<Model, Limit::cachesize>& getSceneModels();
  const Model* requestModelFromScene(const std::string& uuid);
  const bool deleteModelFromScene(const std::string& uuid);
  const std::string addModelToScene(const std::string& path);
  const std::string addModelToScene(Model& m);
  const Texture loadTexture(const std::string& path);

private:
  WestAssetFacade();
  ~WestAssetFacade();
  WestAssetFacade(const WestAssetFacade& other)            = delete;
  WestAssetFacade& operator=(const WestAssetFacade& other) = delete;
  WestAssetFacade(WestAssetFacade&& other)                 = delete;
  WestAssetFacade& operator=(WestAssetFacade&& other)      = delete;

  WestLogger& _logger = WestLogger::getLoggerInstance();
  std::unique_ptr<ResourceController> _controller;
};

};  // namespace WestData
