#include "UIRenderManager.h"

#include <Config.h>
#include <TimeUtils.hpp>

#include <future>
#include <vector>

UIRenderManager::~UIRenderManager() {
  for (ComponentData *cd : data) {
    cd->~ComponentData();
  }
}

void UIRenderManager::updateRenderData(
    std::array<ContainerElement *, 32> interfaces, size_t count) {
  if (count == 0 || interfaces.size() == 0) {
    _logger->log(Level::Error,
                 "@@@ No interfaces to render at least a header should have "
                 "been created - there might be something wrong ... ");
    return;
  }

#ifdef DEBUG
  std::uint8_t cycle = _logger->getCycleLength();
  double start = TimeUtils::getCurrentTimeAsTime();
#endif

  {
    std::lock_guard<std::mutex> lk(_vectorMutex);
    data.clear();
  }

  std::vector<std::future<void>> futures;
  const size_t workers = std::max<size_t>(1, Config::THREADPOOL->getPoolSize());
  const size_t stepsize = (count + workers - 1) / workers;
  futures.reserve((count + stepsize - 1) / stepsize);

  for (size_t begin = 0; begin < count; begin += stepsize) {
    const size_t end = std::min(begin + stepsize, count);
    futures.emplace_back(
        Config::THREADPOOL->enqueue([interfaces, begin, end, this] {
          std::vector<ComponentData *> local;
          for (size_t j = begin; j < end; ++j) {
            if (interfaces[j] == nullptr) {
              _logger->log(
                  Level::Error,
                  std::format("@@@ interface at position {} does not exists.\n",
                              j));
              continue;
            }
            std::vector<ComponentData *> interfaceResult =
                interfaces[j]->describeContainer();
            local.insert(local.end(), interfaceResult.begin(),
                         interfaceResult.end());
          }
          assert(local.size() > 0);
          fillComponentData(local);
        }));
  }

  for (auto &future : futures)
    future.wait();

#ifdef DEBUG
  double end = TimeUtils::getCurrentTimeAsTime();
  double res = TimeUtils::getDuration(start, end);
  if (cycle == 0)
    _logger->log(Level::Cycle,
                 std::format("@@@ Interface Rendering time: {} ms.\n", res));
#endif
}

void UIRenderManager::fillComponentData(std::vector<ComponentData *> cd) {
  std::lock_guard<std::mutex> lk(_vectorMutex);
  data.insert(data.end(), cd.begin(), cd.end());
}
