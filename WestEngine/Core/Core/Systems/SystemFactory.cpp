#include "../../CoreHeaders/Systems/SystemFactory.h"

#include "../../Constants/Systems.h"
#include "../../CoreHeaders/SystemManager.h"
#include "../../CoreHeaders/Systems/Umbrella.h"

void SystemFactory::createSystem(std::map<std::string, std::int32_t> infos,
                                 const std::string name, Entity *e) {

  if (Systems::PLAYER_CONTROL.compare(name) == 0) {
    PlayerControl *c = (PlayerControl *)SystemManager::getSystemByName(
        Systems::PLAYER_CONTROL);
    c->addEntity(e);
  }
}
