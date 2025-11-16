#include "../../CoreHeaders/Entity/Entity.h"

PoolAllocator* Entity::_allocator = new PoolAllocator();

Entity::Entity()
{
  _destroyed     = false;
  _debugEntity   = false;
  _componentMask = {0b0000'0000'0000'0000};
}

Entity::Entity(std::uint32_t id) : _id(id)
{
  _destroyed     = false;
  _debugEntity   = false;
  _componentMask = {0b0000'0000'0000'0000};
}

Entity::~Entity()
{
  delete _allocator;
  _components.clear();
}

IComponent* Entity::getComponent(std::uint16_t componentMask)
{
  if (!static_cast<bool>(componentMask & _componentMask))
  {
    return nullptr;
  }

  return _components.at(componentMask);
}
