#include "../../CoreHeaders/Systems/SystemFactory.h"

#include "../../Constants/Systems.h"
#include "../../CoreHeaders/Systems/Umbrella.h"
#include "../../CoreHeaders/SystemManager.h"

void SystemFactory::createSystem(
    std::map<const char *, std::int32_t, CStrCmp> infos, const char *name,
    Entity *e) {

  if (strcmp(name, Systems::PLAYER_CONTROL) == 0) {
    PlayerControl *c = (PlayerControl *)SystemManager::getSystemByName(
        Systems::PLAYER_CONTROL);
    c->addEntity(e);
  }
}
