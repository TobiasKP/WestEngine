#include "../../CoreHeaders/Components/ComponentFactory.h"

#include "../../Constants/Components.hpp"
#include "../../CoreHeaders/Components/Umbrella.h"

#include <format>

void ComponentFactory::createComponent(std::map<std::string, std::int32_t> infos, std::string name, Entity& e)
{
#ifdef DEBUG
  WestLogger::getLoggerInstance().log(Level::Info, std::format("Adding component: {} to: {}\n", name, e.getId()));
#endif
  if (Components::POSITION.compare(name) == 0)
  {
    // TODO: Add rotation and scale
    Position* p   = new Position();
    glm::vec3 pos = glm::vec3();
    auto it       = infos.find("x");
    if (it != infos.end())
    {
      pos.x = it->second;
    }
    it = infos.find("y");
    if (it != infos.end())
    {
      pos.y = it->second;
    }
    it = infos.find("z");
    if (it != infos.end())
    {
      pos.z = it->second;
    }

    p->position = pos;
    p->scale    = 1.0f;
    p->rotation = glm::vec3(1.0f);
    e.addComponent(BitMasks::Components::POSITION, p);
  }
  else if (Components::MOVEMENT.compare(name) == 0)
  {
    Movement* m = new Movement();
    auto it     = infos.find("r");
    if (it != infos.end())
    {
      m->range = it->second;
    }
    it = infos.find("a");
    if (it != infos.end())
    {
      m->a = (algorithm)it->second;
    }
    e.addComponent(BitMasks::Components::MOVEMENT, m);
  }
}
