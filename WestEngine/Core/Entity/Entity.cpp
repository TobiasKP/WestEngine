#include "../../CoreHeaders/Entity/Entity.h"

PoolAllocator* Entity::_allocator = new PoolAllocator();

Entity::Entity()
{
  _destroyed   = false;
  _debugEntity = false;
  _activeUnit  = false;
  _initialized = false;
}

Entity::Entity(std::uint32_t id) : _id(id)
{
  _destroyed   = false;
  _debugEntity = false;
  _activeUnit  = false;
  _initialized = false;
}

Entity::~Entity() {}


Entity::Entity(const Entity& other)
  : _id(other._id), _shaderId(other._shaderId), _name(other._name), _modelGuid(other._modelGuid),
    _destroyed(other._destroyed), _debugEntity(other._debugEntity), _activeUnit(other._activeUnit),
    _initialized(other._initialized)
{}

Entity& Entity::operator=(const Entity& other)
{
  if (this != &other)
  {
    _id          = other._id;
    _shaderId    = other._shaderId;
    _name        = other._name;
    _modelGuid   = other._modelGuid;
    _destroyed   = other._destroyed;
    _debugEntity = other._debugEntity;
    _activeUnit  = other._activeUnit;
    _initialized = other._initialized;
  }
  return *this;
}

Entity::Entity(Entity&& other) noexcept
  : _id(other._id), _shaderId(other._shaderId), _name(std::move(other._name)),
    _modelGuid(std::move(other._modelGuid)), _destroyed(other._destroyed), _debugEntity(other._debugEntity),
    _activeUnit(other._activeUnit), _initialized(other._initialized)
{
  other._destroyed   = false;
  other._debugEntity = false;
  other._activeUnit  = false;
  other._initialized = false;
}

Entity& Entity::operator=(Entity&& other) noexcept
{
  if (this != &other)
  {
    _id                = other._id;
    _shaderId          = other._shaderId;
    _name              = std::move(other._name);
    _modelGuid         = std::move(other._modelGuid);
    _destroyed         = other._destroyed;
    _debugEntity       = other._debugEntity;
    _activeUnit        = other._activeUnit;
    _initialized       = other._initialized;
    other._destroyed   = false;
    other._debugEntity = false;
    other._activeUnit  = false;
    other._initialized = false;
  }
  return *this;
}
