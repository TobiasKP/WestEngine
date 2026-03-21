#include "../CoreHeaders/SystemManager.h"

#include "../CoreHeaders/Systems/Umbrella.h"

#include <algorithm>
#include <Config.h>
#include <format>

std::array<ISystem*, 4> SystemManager::_systems = {};

SystemManager::SystemManager() : IManager(nullptr)
{
  _dispatcher = nullptr;
  setName(CoreConstants::ENTITY_SYSTEM_MANAGER);
}

SystemManager::SystemManager(WestLogger* logger, const std::shared_ptr<EventDispatcher>& d, const std::shared_ptr<Scene>& s)
  : IManager(logger)
{
  _scene      = s;
  _dispatcher = d;
  setName(CoreConstants::ENTITY_SYSTEM_MANAGER);
}

SystemManager::~SystemManager() {}

std::int32_t SystemManager::startup()
{
  std::shared_ptr<Camera> c            = _scene->getCamera();
  std::shared_ptr<ComponentRegistry> r = _scene->getRegistry();
  _systems                             = {new PlayerControl(_dispatcher, getLogger(), r, c),
                                          new MovementSystem(_dispatcher, getLogger(), r),
                                          new CameraSystem(_dispatcher, getLogger(), r, c),
                                          new PositionalSystem(_dispatcher, getLogger(), r, c)};


#ifdef DEBUG
  logDebug(std::format("{} ### Instantiated critical game systems\n", getName()));
#endif
  return 0;
}

void SystemManager::shutdown()
{
#ifdef DEBUG
  logDebug(std::format("{} ### Shutting down {}...\n", getName(), getName()));
#endif
}

std::int32_t SystemManager::init()
{
#ifdef DEBUG
  double start = TimeUtils::getCurrentTimeAsTime();
#endif

  std::shared_ptr<World> w = _scene->getWorld();
  for (ISystem* sys : _systems)
  {
    sys->init(w);
  }

#ifdef DEBUG
  double end = TimeUtils::getCurrentTimeAsTime();
  double res = TimeUtils::getDuration(start, end);
  logDebug(std::format("{} ### EntitySystemManager init time: {} ms.\n", getName(), res));
#endif

  return 0;
}

void SystemManager::update()
{
#ifdef DEBUG
  std::uint8_t cycle = getLogger()->getCycleLength();
  double start       = TimeUtils::getCurrentTimeAsTime();
  for (ISystem* system : _systems)
  {
    system->updateDebuggingInfo();
  }
#endif 
  std::vector<std::future<void>> futures;
  for (ISystem* system : _systems)
  {
    futures.emplace_back(Config::THREADPOOL->enqueue(
      [system, logger = getLogger()]
      {
        if (system == nullptr)
        {
          logger->log(Level::Error, "System invalid null ptr check entity file or debug\n");
          return;
        } 
        system->update();
      }));
  }

#ifdef DEBUG
  std::int32_t count = futures.size();
#endif

  for (auto& future : futures)
  {
    future.wait();
  }

#ifdef DEBUG
  double end  = TimeUtils::getCurrentTimeAsTime();
  double res  = TimeUtils::getDuration(start, end);
  _avgTime   += res;
  if (cycle == 0)
  {
    logCycle(std::format("{} ### EntitySystemManager run {} cycles for all "
                         "entities: (number of entites) {} - average time per cycle: {} ms.\n",
                         getName(),
                         cycle,
                         count,
                         _avgTime / cycle));
    _avgTime = 0;
  }
#endif
}

// TODO Change to IndexBased or BitBased lookup
ISystem* SystemManager::getSystemByName(const std::string_view name)
{
  auto it = std::find_if(_systems.begin(),
                         _systems.end(),
                         [name](ISystem* obj)
                         {
                           if (obj == nullptr)
                           {
                             return false;
                           }
                           return name.compare(obj->getName().c_str()) == 0;
                         });

  if (it != _systems.end())
  {
    return (*it);
  }

  return nullptr;
}
