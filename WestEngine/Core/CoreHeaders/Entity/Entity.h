#pragma once

#include <GL/glew.h>

#include <unordered_map>

#include "../Interfaces/IComponent.h"
#include <PoolAllocator.h>

class Entity {
public:
  Entity();
  Entity(std::uint32_t id);
  ~Entity();

  // Getter
  inline std::uint32_t getId() { return _id; }
  inline bool isDestroyed() { return _destroyed; }
  inline bool isDebugEntity() { return _debugEntity; }
  inline char *getName() { return _name; }

  // Setter
  inline void destroy() { _destroyed = true; }
  inline void debugEntity() { _debugEntity = true; }
  inline void setId(std::uint32_t id) { _id = id; }
  inline void setName(char *name) { _name = name; }

  // Overrides
  static void *operator new(size_t size) { return _allocator->allocate(size); }
  static void operator delete(void *ptr, size_t size) {
    return _allocator->deallocate(ptr, size);
  }

  // Functions
  void addComponent(std::uint16_t flag, IComponent *component) {
    _componentMask |= flag;
    _components.insert(std::make_pair(flag, component));
  }
  IComponent *getComponent(std::uint16_t componentMask);

private:
  static PoolAllocator *_allocator;

  std::uint32_t _id;
  char *_name;
  std::uint16_t _componentMask;
  std::unordered_map<std::uint16_t, IComponent *> _components;
  bool _destroyed, _debugEntity;
};
