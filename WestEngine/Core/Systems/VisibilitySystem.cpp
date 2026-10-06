#include "../../CoreHeaders/Systems/VisibilitySystem.hpp"

VisibilitySystem::VisibilitySystem(std::shared_ptr<EventDispatcher> d,
                                   WestLogger* l,
                                   std::shared_ptr<ComponentRegistry> r)
  : ISystem(d, l, r)
{}

VisibilitySystem::~VisibilitySystem() {}

void VisibilitySystem::update()
{
  _origins.clear();
  std::shared_ptr<ComponentArray<Control>> controls = _reg->getComponentArray<Control>();
  size_t size                                       = controls->getSize();
  for (size_t i = 0; i < size; i++)
  {
    Control* c = controls->getComponentByIdx(i);
    if (c->aiControl)
    {
      continue;
    }
    std::uint32_t id                  = controls->getEntityIdByIdx(i);
    Position* posComp                 = _reg->getComponent<Position>(id);
    std::optional<std::int32_t> range = getLineOfSightRange(_reg->getComponent<LineOfSight>(id),
                                                            _reg->getComponent<Movement>(id));
    if (!posComp || !range.has_value())
    {
      continue;
    }
    _origins.emplace_back(_world->calculateIndex(posComp->position.x, posComp->position.z), range.value());
  }
  _world->updateVisibility(_origins);
}

void VisibilitySystem::updateDebuggingInfo() {}

void VisibilitySystem::init(const std::shared_ptr<World>& w)
{
  _world = w;
}
