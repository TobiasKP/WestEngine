#include "DataPool.hpp"

DataPool::DataPool() {}

DataPool::~DataPool() {}

void DataPool::init() {}

void DataPool::shutdown() {}

const Model* DataPool::getNextFreeProjectile(const std::string& type)
{
  return nullptr;
};

const Model* DataPool::getModelByGuid(const std::string& guid)
{
  return nullptr;
};

const Model* DataPool::getModelByName(const std::string& name)
{
  return nullptr;
}

std::array<Model, Limit::cachesize>& DataPool::getSceneModels()
{
  return _sceneModels;
};

bool DataPool::addModelToScene(Model&& m)
{
  return true;
};

bool DataPool::deleteModelFromScene(const std::string& guid)
{
  return true;
};
