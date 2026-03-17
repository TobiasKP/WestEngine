#include "../../CoreHeaders/Systems/PositionalSystem.hpp"

#include "../../CoreHeaders/Entity/Scene.h"
#include "../CoreHeaders/Utils/Math/PositionCalculation.h"

PositionalSystem::PositionalSystem(WestLogger* logger, std::shared_ptr<EventDispatcher> d)
{
  _logger     = logger;
  _dispatcher = d;
  _lastEntity = -1;
};


PositionalSystem::~PositionalSystem() {};

void PositionalSystem::update()
{
  pollEvents();
  std::shared_ptr<ComponentArray<Position>> pos = _reg->getComponentArray<Position>();
  World* w                                      = Scene::getSceneInstance().getWorld();
  std::uint32_t id                              = w->getEntityByIdx(_tileIdx);
  Material* m;

  //TODO: Bug -> if emissiveColor is ever used this will overwrite it
  if (_lastEntity != -1 && _lastEntity != id)
  {
    m                = Scene::getSceneInstance().getRegistry()->getComponent<Material>(_lastEntity);
    m->emissiveColor = glm::vec3(0.0, 0.0, 0.0);
  }
  if (id > 0)
  { 
    m                = Scene::getSceneInstance().getRegistry()->getComponent<Material>(id);
    m->emissiveColor = glm::vec3(0.0, 0.5, 0.5);
    _lastEntity      = id;
  }
};

void PositionalSystem::updateDebuggingInfo() {};

void PositionalSystem::init()
{
  _reg = Scene::getSceneInstance().getRegistry();
  _dispatcher->subscribe(EventIdentifiers::MOUSE_MOVE,
                         [this](EventIdentifiers event, EventPayload payload) { pushEvent(event, payload); });
};

void PositionalSystem::pollEvents()
{
  std::vector<std::tuple<EventIdentifiers, EventPayload>> events = _eventQueue.drain();

  // Only the last mouse position matters for hover — skip redundant intermediate events
  MousePayload* lastMouse = nullptr;
  for (std::tuple<EventIdentifiers, EventPayload> event : events)
  {
    if (std::get<0>(event) == EventIdentifiers::MOUSE_MOVE)
    {
      lastMouse = std::get_if<MousePayload>(&std::get<1>(event));
    }
  }

  if (lastMouse)
  {
    glm::vec3 hoverPosition = PositionCalculation::getWorldPosition(glm::vec2(lastMouse->x, lastMouse->y),
                                                                    Scene::getSceneInstance().getCamera());
    _tileIdx                = Scene::getSceneInstance().getWorld()->worldPosToTile(hoverPosition.x, hoverPosition.z);
  }
}
