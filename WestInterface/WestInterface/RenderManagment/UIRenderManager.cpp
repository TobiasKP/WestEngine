#include "UIRenderManager.h"

#include <Config.h>

#include <future>
#include <vector>

UIRenderManager::~UIRenderManager() {
  for (ComponentData *cd : data) {
    cd->~ComponentData();
  }
}

void UIRenderManager::updateRenderData(
    std::array<ContainerElement *, 32> interfaces, size_t count) {
  if(count == 0) {
    return;
  }

  std::vector<std::future<void>> futures;
  const size_t workers =
      std::max<size_t>(1, Config::THREADPOOL->getPoolSize());
  const size_t stepsize = (count + workers - 1) / workers;
  futures.reserve((count + stepsize - 1) / stepsize);

  for (size_t begin = 0; begin < count; begin += stepsize) {
    const size_t end = std::min(begin + stepsize, count);

    futures.emplace_back(Config::THREADPOOL->enqueue(
        [interfaces, begin, end, this] {
          std::vector<ComponentData *> local;
          for (size_t j = begin; j < end; ++j) {
            std::vector<ComponentData *> interfaceResult =
                interfaces[j]->describeContainer();
            local.insert(local.end(), interfaceResult.begin(),
                         interfaceResult.end());
          }
          fillComponentData(local);
        }));
  }

  for (auto &future : futures)
    future.wait();
}

void UIRenderManager::fillComponentData(std::vector<ComponentData *> cd) {
  std::lock_guard<std::mutex> lk(_vectorMutex);
  data.insert(data.end(), cd.begin(), cd.end());
}
