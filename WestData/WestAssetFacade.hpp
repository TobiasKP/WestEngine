#pragma once

#include "Data/Model.hpp"

#include <vector>

class WestAssetFacade
{
public:
  static WestAssetFacade& getAssetFacade();

  const std::vector<Model>& getSceneModels();
  const Model& requestModelFromScene(const std::string& uuid);
  const bool deleteModelFromScene(const std::string& uuid);
  const std::string& addModelToScene(const std::string& path);

private:
  WestAssetFacade();
  ~WestAssetFacade();
  WestAssetFacade(const WestAssetFacade& other)            = delete;
  WestAssetFacade& operator=(const WestAssetFacade& other) = delete;
  WestAssetFacade(WestAssetFacade&& other)                 = delete;
  WestAssetFacade& operator=(WestAssetFacade&& other)      = delete;
};
