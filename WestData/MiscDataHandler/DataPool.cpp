#include "DataPool.hpp"

DataPool::DataPool() {}

DataPool::~DataPool() {}

void DataPool::init() {}

void DataPool::shutdown() {}

const Model* DataPool::getNextFreeProjectile(const std::string& type)
{
  return nullptr;
};

const Model* DataPool::getModelByGuid(const std::string& guid) {};

std::array<Model, Limit::cachesize>& DataPool::getSceneModels() {};

bool DataPool::addModelToScene(Model&& m) {};

bool DataPool::deleteModelFromScene(const std::string& guid) {};
