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

  std::vector<std::future<void>> futures;
  for (ISystem *system : _systems) {
    futures.emplace_back(Config::EngineInternals.THREADPOOL->enqueue(
        [system, logger = getLogger()] {
          if (system == nullptr) {
            logger->log(Level::Error,
                        "System invalid null ptr check entity file or debug\n");
            return;
          }
          system->update();
        }));
  }

#ifdef DEBUG
  std::int32_t count = futures.size();
#endif

  for (auto &future : futures)
    future.wait();




  for (size_t i = 0; i < count; ++i) {
    data.push_back(interfaces[i]->describe());
  }
}
