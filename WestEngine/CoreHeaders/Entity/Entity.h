#pragma once

#include "../Interfaces/IComponent.h"

#include <GL/glew.h>
#include <PoolAllocator.h>
#include <unordered_map>

class Entity
{
public:
  Entity();
  Entity(std::uint32_t id);
  ~Entity();

  // Copy constructor and assignment
  Entity(const Entity& other);
  Entity& operator=(const Entity& other);

  // Move constructor and assignment
  Entity(Entity&& other) noexcept;
  Entity& operator=(Entity&& other) noexcept;

  // Getter
  inline std::uint32_t getId() const
  {
    return _id;
  }

  inline bool isDestroyed() const
  {
    return _destroyed;
  }

  inline bool isDebugEntity() const
  {
    return _debugEntity;
  }

  inline char* getName() const
  {
    return _name;
  }

  // Setter
  inline void destroy()
  {
    _destroyed = true;
  }

  inline void debugEntity()
  {
    _debugEntity = true;
  }

  inline void setId(std::uint32_t id)
  {
    _id = id;
  }

  inline void setName(char* name)
  {
    _name = name;
  }

  // Overrides
  static void* operator new(size_t size)
  {
    return _allocator->allocate(size);
  }

  static void operator delete(void* ptr, size_t size)
  {
    return _allocator->deallocate(ptr, size);
  }

  // Functions
  void addComponent(std::uint16_t flag, IComponent* component);
  IComponent* getComponent(std::uint16_t componentMask) const;

private:
  static PoolAllocator* _allocator;

  std::uint32_t _id = 0;
  std::uint16_t _componentMask;
  bool _destroyed, _debugEntity;
  char* _name;
  std::unordered_map<std::uint16_t, IComponent*> _components;
};
