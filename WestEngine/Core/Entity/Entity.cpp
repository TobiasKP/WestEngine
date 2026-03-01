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

Entity::~Entity() {}


Entity::Entity(const Entity& other)
  : _id(other._id), _name(other._name), _componentMask(other._componentMask), _destroyed(other._destroyed),
    _debugEntity(other._debugEntity)
{}

Entity& Entity::operator=(const Entity& other)
{
  if (this != &other)
  {
    _id            = other._id;
    _name          = other._name;
    _componentMask = other._componentMask;
    _destroyed     = other._destroyed;
    _debugEntity   = other._debugEntity;
  }
  return *this;
}

Entity::Entity(Entity&& other) noexcept
  : _id(other._id), _name(other._name), _componentMask(other._componentMask), _destroyed(other._destroyed),
    _debugEntity(other._debugEntity)
{
  other._componentMask = 0;
  other._destroyed     = false;
  other._debugEntity   = false;
}

Entity& Entity::operator=(Entity&& other) noexcept
{
  if (this != &other)
  {
    _id                  = other._id;
    _name                = other._name;
    _componentMask       = other._componentMask;
    _destroyed           = other._destroyed;
    _debugEntity         = other._debugEntity;
    other._componentMask = 0;
    other._destroyed     = false;
    other._debugEntity   = false;
  }
  return *this;
}
