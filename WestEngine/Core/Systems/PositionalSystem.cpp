#include "../../CoreHeaders/Systems/PositionalSystem.hpp"

#include "../CoreHeaders/Utils/Math/PositionCalculation.h"

PositionalSystem::PositionalSystem(std::shared_ptr<EventDispatcher> d,
                                   WestLogger* l,
                                   std::shared_ptr<ComponentRegistry> r,
                                   std::shared_ptr<Camera> c)
  : ISystem(d, l, r)
{
  _cam          = c;
  _lastEntity   = -1;
  _tileIdx      = -1;
  _lastEmissive = glm::vec3(0);
  _highlighted  = false;
};

PositionalSystem::~PositionalSystem() {};

void PositionalSystem::update()
{
  pollEvents();
  std::shared_ptr<ComponentArray<Position>> pos = _reg->getComponentArray<Position>();
  std::uint32_t id                              = _world->getEntityByIdx(_tileIdx);
  Material* m;

  // TODO: Bug -> if emissiveColor is ever used this will overwrite it
  if (_lastEntity != -1 && _lastEntity != id)
  {
    m = _reg->getComponent<Material>(_lastEntity);
    assert(m != nullptr);
    m->emissiveColor = _lastEmissive;
    _lastEmissive    = glm::vec3(0);
    _highlighted     = false;
  }
  if (id > 0 && !_highlighted)
  {
    m = _reg->getComponent<Material>(id);
    assert(m != nullptr);
    _lastEmissive    = m->emissiveColor;
    m->emissiveColor += glm::vec3(0.0, 0.5, 0.5);
    _lastEntity      = id;
    _highlighted     = true;
  }
};

void PositionalSystem::updateDebuggingInfo() {};

void PositionalSystem::init(const std::shared_ptr<World>& w)
{
  _world = w;
  _dispatcher->subscribe(EventIdentifiers::MOUSE_MOVE,
                         [this](EventIdentifiers event, EventPayload payload) { pushEvent(event, payload); });
};

void PositionalSystem::pollEvents()
{
  std::vector<std::tuple<EventIdentifiers, EventPayload>> events = _eventQueue.drain();

  // Only the last mouse position matters for hover — skip redundant intermediate events
  MousePayload* lastMouse = nullptr;
  for (std::tuple<EventIdentifiers, EventPayload>& event : events)
  {
    if (std::get<0>(event) == EventIdentifiers::MOUSE_MOVE)
    {
      lastMouse = std::get_if<MousePayload>(&std::get<1>(event));
    }
  }

  if (lastMouse)
  {
    glm::vec3 hoverPosition = PositionCalculation::getWorldPosition(glm::vec2(lastMouse->x, lastMouse->y), _cam);
    _tileIdx                = _world->calculateIndex(hoverPosition.x, hoverPosition.z);
  }
}
