#include "../CoreHeaders/SystemManager.h"
#include "../Config/Config.h"

#include <algorithm>

#include "../CoreHeaders/Systems/Umbrella.h"
#include "../CoreHeaders/Utils/TimeUtils.h"

std::array<ISystem *, 1> SystemManager::_systems = {};

SystemManager::SystemManager() : IManager(nullptr) {
  setName(CoreConstants::ENTITY_SYSTEM_MANAGER);
}

SystemManager::SystemManager(WestLogger *logger) : IManager(logger) {
  setName(CoreConstants::ENTITY_SYSTEM_MANAGER);
}

SystemManager::~SystemManager() {}

std::int32_t SystemManager::startup() {
  _systems = {new PlayerControl(getLogger())};

  return 0;
}

void SystemManager::shutdown() {
#ifdef DEBUG
  getString()->format("%s ### Shutting down %s...\n", getName(), getName());
  logDebug(getString()->getBuffer());
#endif
}

std::int32_t SystemManager::init() {
#ifdef DEBUG
  double start = TimeUtils::getCurrentTimeAsTime();
#endif

  _scene = &Scene::getSceneInstance();

#ifdef DEBUG
  double end = TimeUtils::getCurrentTimeAsTime();
  double res = TimeUtils::getDuration(start, end);
  getString()->format("%s ### EntitySystemManager init time: %f ms.\n",
                      getName(), res);
  logDebug(getString()->getBuffer());
#endif

  return 0;
}

void SystemManager::update() {
#ifdef DEBUG
  _loggingFrequence++;
  double start = TimeUtils::getCurrentTimeAsTime();
  for (ISystem *system : _systems) {
    //TODO fix flickering artifact when updating debugging info in playercontrol
    system->updateDebuggingInfo();
  }
#endif

  std::vector<std::future<void>> futures;
  for (ISystem *system : _systems) {
    futures.emplace_back(
       Config::EngineInternals.THREADPOOL->enqueue([system, logger = getLogger()] {
          if (system == nullptr) {
            logger->writeError(
                "System invalid null ptr check entity file or debug\n");
            return;
          }
          system->update();
        }));
  }

  std::int32_t count = futures.size();
  for (auto &future : futures)
    future.wait();

#ifdef DEBUG
  double end = TimeUtils::getCurrentTimeAsTime();
  double res = TimeUtils::getDuration(start, end);
  _avgTime = ((_avgTime * (_loggingFrequence - 1)) + res) / _loggingFrequence;
  if (_loggingFrequence == 50) {
    getString()->format(
        "%s ### EntitySystemManager run %d cycles for all "
        "entities: (number of entites) %d - average time per cycle: %f ms.\n",
        getName(), _loggingFrequence, count, _avgTime);
    logDebug(getString()->getBuffer());
    _loggingFrequence = 0;
    _avgTime = 0;
  }
#endif
}

//TODO Change to IndexBased or BitBased lookup
ISystem *SystemManager::getSystemByName(const char *name) {
  auto it =
      std::find_if(_systems.begin(), _systems.end(), [name](ISystem *obj) {
        if (obj == nullptr)
          return false;
        return strcmp(name, obj->getName()) == 0;
      });

  if (it != _systems.end()) {
    return (*it);
  }

  return nullptr;
}
