#include "../../CoreHeaders/Entity/Entity.h"

#include <bitset>
#include <format>

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
  _components.clear();
}


Entity::Entity(const Entity& other)
  : _id(other._id), _name(other._name), _componentMask(other._componentMask), _destroyed(other._destroyed),
    _debugEntity(other._debugEntity)
{
  _components = other._components;
}

Entity& Entity::operator=(const Entity& other)
{
  if (this != &other)
  {
    _id            = other._id;
    _name          = other._name;
    _componentMask = other._componentMask;
    _destroyed     = other._destroyed;
    _debugEntity   = other._debugEntity;
    _components    = other._components;
  }
  return *this;
}

Entity::Entity(Entity&& other) noexcept
  : _id(other._id), _name(other._name), _componentMask(other._componentMask), _destroyed(other._destroyed),
    _debugEntity(other._debugEntity), _components(std::move(other._components))
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
    _components          = other._components;
    other._componentMask = 0;
    other._destroyed     = false;
    other._debugEntity   = false;
  }
  return *this;
}

void Entity::addComponent(std::uint16_t flag, IComponent* component)
{
  _componentMask |= flag;
  _components.insert(std::make_pair(flag, component));
}

IComponent* Entity::getComponent(std::uint16_t componentMask) const
{
  if (!static_cast<bool>(componentMask & _componentMask))
  {
#ifdef DEBUG
    if (!_debugEntity)
    {
      WestLogger::getLoggerInstance().log(
        Level::Error,
        std::format("Error retrieveng component: 0b{} from entity: {}, given components: 0b{}\n",
                    std::bitset<16>(componentMask).to_string(),
                    _id,
                    std::bitset<16>(_componentMask).to_string()));
    }
#endif
    return nullptr;
  }

  return _components.at(componentMask);
}
