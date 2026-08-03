#include "../../CoreHeaders/Systems/PositionalSystem.hpp"

#include "../CoreHeaders/Utils/Math/PositionCalculation.h"

PositionalSystem::PositionalSystem(std::shared_ptr<EventDispatcher> d,
                                   WestLogger* l,
                                   std::shared_ptr<ComponentRegistry> r,
                                   std::shared_ptr<Camera> c)
  : ISystem(d, l, r)
{
  _cam         = c;
  _lastEntity  = -1;
  _tileIdx     = -1;
  _highlighted = false;
};

PositionalSystem::~PositionalSystem() {};

void PositionalSystem::update()
{
  pollEvents();
  if (_lastMouse.has_value())
  {
    glm::vec3 hoverPosition = PositionCalculation::getWorldPosition(glm::vec2(_lastMouse->x, _lastMouse->y), _cam);
    _tileIdx                = _world->calculateIndex(hoverPosition.x, hoverPosition.z);
  }
  std::shared_ptr<ComponentArray<Position>> pos = _reg->getComponentArray<Position>();
  std::uint32_t id                              = _world->getEntityByIdx(_tileIdx);

  if (_lastEntity != -1 && _lastEntity != id)
  {
    Appearance* a = _reg->getComponent<Appearance>(_lastEntity);
    if (a)
    {
      a->emissiveOverride = glm::vec3(0.0f);
    }
    _lastEntity  = -1;
    _highlighted = false;
  }
  if (id > 0 && !_highlighted)
  {
    Appearance* a = _reg->getComponent<Appearance>(id);
    if (a)
    {
      a->emissiveOverride = glm::vec3(0.0, 0.5, 0.5);
      _lastEntity         = id;
      _highlighted        = true;
    }
  }
};

void PositionalSystem::updateDebuggingInfo() {};

void PositionalSystem::init(const std::shared_ptr<World>& w)
{
  _world = w;
  _dispatcher->subscribe(EventIdentifiers::MOUSE_MOVE,
                         [this](EventIdentifiers event, EventPayload payload) { pushEvent(event, payload); });
};

void PositionalSystem::handleEvent(std::tuple<EventIdentifiers, EventPayload> event)
{
  if (std::get<0>(event) == EventIdentifiers::MOUSE_MOVE)
  {
    MousePayload* p = std::get_if<MousePayload>(&std::get<1>(event));
    if (p)
    {
      _lastMouse = *p;
    }
  }
}
