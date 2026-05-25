#pragma once

#include "Data/Model.hpp"

#include <vector>

class WestAssetFacade
{
public:
  static WestAssetFacade& getAssetFacade();

  const std::vector<Model>& getScene();
  const Model& requestModel(const std::string& uuid);
  const bool deleteModel(const std::string& uuid);
  const void addModel(const std::string& path);

private:
  WestAssetFacade();
  ~WestAssetFacade();
  WestAssetFacade(const WestAssetFacade& other)            = delete;
  WestAssetFacade& operator=(const WestAssetFacade& other) = delete;
  WestAssetFacade(WestAssetFacade&& other)                 = delete;
  WestAssetFacade& operator=(WestAssetFacade&& other)      = delete;
};
