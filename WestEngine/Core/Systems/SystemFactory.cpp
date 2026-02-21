#include "../../CoreHeaders/Systems/SystemFactory.h"

#include "../../Constants/Systems.hpp"
#include "../../CoreHeaders/SystemManager.h"
#include "../../CoreHeaders/Systems/Umbrella.h"

#include <format>

void SystemFactory::createSystem(std::map<std::string, std::int32_t> infos, const std::string name, const Entity& e)
{
#ifdef DEBUG
  WestLogger::getLoggerInstance().log(Level::Info, std::format("Adding system: {} to: {}\n", name, e.getId()));
#endif
  if (Systems::PLAYER_CONTROL.compare(name) == 0)
  {
    PlayerControl* c = (PlayerControl*)SystemManager::getSystemByName(Systems::PLAYER_CONTROL.data());
    assert(c != nullptr);
    c->addEntity(e);
  }
  else if (Systems::MOVEMENT.compare(name) == 0)
  {
    MovementSystem* m = (MovementSystem*)SystemManager::getSystemByName(Systems::MOVEMENT.data());
    assert(m != nullptr);
    m->addEntity(e);
  }
}
