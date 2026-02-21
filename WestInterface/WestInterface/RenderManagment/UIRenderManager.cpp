#include "UIRenderManager.h"

#include <algorithm>
#include <Config.h>
#include <format>
#include <future>
#include <TimeUtils.hpp>
#include <vector>

UIRenderManager::~UIRenderManager()
{
  for (ComponentData* cd : _data)
  {
    cd->~ComponentData();
  }
}

void UIRenderManager::updateRenderData(std::array<ContainerElement*, 32>& interfaces, size_t count)
{
  if (count == 0 || interfaces.size() == 0)
  {
    _logger->log(Level::Error,
                 "@@@ No interfaces to render at least a header should have "
                 "been created - there might be something wrong ... ");
    return;
  }

  if (!_dirty)
  {
    return;
  }

#ifdef DEBUG
  std::uint8_t cycle = _logger->getCycleLength();
  double start       = TimeUtils::getCurrentTimeAsTime();
#endif

  {
    std::lock_guard<std::mutex> lk(_vectorMutex);
    _data.clear();
    _boundaryData.clear();
  }

  std::vector<std::future<void>> futures;
  const size_t workers  = std::max<size_t>(1, Config::THREADPOOL->getPoolSize());
  const size_t stepsize = (count + workers - 1) / workers;
  futures.reserve((count + stepsize - 1) / stepsize);

  for (size_t begin = 0; begin < count; begin += stepsize)
  {
    const size_t end = std::min(begin + stepsize, count);
    futures.emplace_back(Config::THREADPOOL->enqueue(
      [&interfaces, begin, end, this]
      {
        std::vector<ComponentData*> local;
        std::vector<ElementBounds*> localB;
        gatherUIData(local, interfaces, begin, end);
        gatherBoundaryData(localB, interfaces, begin, end);
        fillComponentData(local, localB);
      }));
  }

  for (auto& future : futures)
  {
    future.wait();
  }
  _dirty = false;

#ifdef DEBUG
  double end = TimeUtils::getCurrentTimeAsTime();
  double res = TimeUtils::getDuration(start, end);
  if (cycle == 0)
  {
    _logger->log(Level::Cycle, std::format("@@@ Interface Rendering time: {} ms.\n", res));
  }
#endif
}

void UIRenderManager::gatherBoundaryData(std::vector<ElementBounds*>& local,
                                         const std::array<ContainerElement*, 32>& interfaces,
                                         size_t begin,
                                         size_t end)
{
  for (size_t j = begin; j < end; ++j)
  {
    ContainerElement* ce = interfaces[j];
    if (ce == nullptr)
    {
      break;
    }
    for (IElement* el : ce->children)
    {
      ElementBounds* b = new ElementBounds();
      b->id            = -1;
      el->getBoundaries(*b);
      if (b->id >= 0)
      {
        local.push_back(b);
      }
    }
    ElementBounds* b = new ElementBounds();
    ce->getBoundaries(*b);
    local.push_back(b);
  }
}

void UIRenderManager::gatherUIData(std::vector<ComponentData*>& local,
                                   const std::array<ContainerElement*, 32>& interfaces,
                                   size_t begin,
                                   size_t end)
{
  for (size_t j = begin; j < end; ++j)
  {
    if (interfaces[j] == nullptr)
    {
      _logger->log(Level::Error, std::format("@@@ interface at position {} does not exists.\n", j));
      continue;
    }
    std::vector<ComponentData*> interfaceResult = interfaces[j]->describeContainer();
    local.insert(local.end(), interfaceResult.begin(), interfaceResult.end());
  }
  assert(local.size() > 0);
}

void UIRenderManager::fillComponentData(std::vector<ComponentData*>& cd, std::vector<ElementBounds*>& bounds)
{
  std::lock_guard<std::mutex> lk(_vectorMutex);
  _data.insert(_data.end(), cd.begin(), cd.end());
  std::stable_sort(
    _data.begin(), _data.end(), [](const ComponentData* a, const ComponentData* b) { return a->zIndex < b->zIndex; });
  _boundaryData.insert(_boundaryData.end(), bounds.begin(), bounds.end());
  std::stable_sort(_boundaryData.begin(),
                   _boundaryData.end(),
                   [](const ElementBounds* a, const ElementBounds* b) { return a->zIndex > b->zIndex; });
}
