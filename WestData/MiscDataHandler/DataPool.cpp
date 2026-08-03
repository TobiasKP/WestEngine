#include "DataPool.hpp"
#include <format>

DataPool::DataPool(WestLogger* l)
{
  _logger     = l;
  _currentIdx = 0;
}

DataPool::~DataPool() {}

void DataPool::init() {}

void DataPool::shutdown() {}

const Model* DataPool::getNextFreeProjectile(const std::string& type)
{
  return nullptr;
};

const Model* DataPool::getModelByGuid(const std::string& guid)
{
  auto idx = _guidToIndex.find(guid);
#ifdef DEBUG
  _logger->log(Level::Info, std::format("|*| Models in scene: {}. Looking for: {}\n", _guidToIndex.size(), guid));
#endif
  if (idx == _guidToIndex.end())
  {
    return nullptr;
  }
  return &_sceneModels.at(idx->second);
};

const Model* DataPool::getModelByName(const std::string& name)
{
  auto guid = _nameToGuid.find(name);
#ifdef DEBUG
  _logger->log(Level::Info, std::format("|*| Models in scene: {}.\n", _guidToIndex.size()));
#endif
  if (guid == _nameToGuid.end())
  {
    return nullptr;
  }
  return getModelByGuid(guid->second);
}

std::array<Model, Limit::cachesize>& DataPool::getSceneModels()
{
  return _sceneModels;
};

bool DataPool::addModelToScene(Model&& m)
{
  if (_sceneModels.max_size() - 1 == _currentIdx)
  {
    _logger->log(Level::Error, "|*| Model array full not more entities supported for the scene\n");
    return false;
  }
  _guidToIndex[m.getGuid()] = _currentIdx;
  _nameToGuid[m.getName()]  = m.getGuid();
#ifdef DEBUG
  _logger->log(Level::Info, std::format("|*| Adding model '{}' with GUID '{}' at index {}\n", m.getName(), m.getGuid(), _currentIdx));
#endif
  _sceneModels[_currentIdx] = std::move(m);
  _currentIdx++;
  return true;
};

bool DataPool::deleteModelFromScene(const std::string& guid)
{
  const Model* m = getModelByGuid(guid);
  if (m == nullptr)
  {
    _logger->log(Level::Error,
                 std::format("|*| Can not remove: {} from scene, model not present with this guid\n", guid));
    return false;
  }
  _nameToGuid.erase(m->getName());
  std::uint32_t idx             = _guidToIndex[guid];
  _sceneModels[idx]             = _sceneModels[_currentIdx - 1];
  _sceneModels[_currentIdx - 1] = {};
  _currentIdx--;
  _guidToIndex[_sceneModels[idx].getGuid()] = idx;
  _guidToIndex.erase(guid);
  return true;
};
