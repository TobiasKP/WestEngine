#pragma once

#include "../Interfaces/IComponentArray.hpp"
#include "ComponentArray.hpp"
#include "Umbrella.h"

#include <memory>
#include <typeinfo>
#include <unordered_map>

class ComponentRegistry
{
public:
  template <typename T>
  void registerComponent()
  {
    const std::type_info& t        = typeid(T);
    _componentArrays[t.hash_code()] = std::make_shared<ComponentArray<T>>();
  };

  template <typename T>
  void addComponent(std::uint32_t entityId, T&& component)
  {
    const std::type_info& t                = typeid(T);
    std::shared_ptr<IComponentArray> array = _componentArrays[t.hash_code()];
    std::static_pointer_cast<ComponentArray<T>>(array)->addComponent(entityId, std::forward<T>(component));
  };

  template <typename T>
  bool removeComponent(std::uint32_t entityId)
  {
    const std::type_info& t                = typeid(T);
    std::shared_ptr<IComponentArray> array = _componentArrays[t.hash_code()];
    return std::static_pointer_cast<ComponentArray<T>>(array)->remove(entityId);
  };

  template <typename T>
  T* getComponent(std::uint32_t entityId)
  {
    const std::type_info& t                = typeid(T);
    std::shared_ptr<IComponentArray> array = _componentArrays[t.hash_code()];
    return std::static_pointer_cast<ComponentArray<T>>(array)->getComponentById(entityId);
  };


  template <typename T>
  std::shared_ptr<ComponentArray<T>> getComponentArray()
  {
    const std::type_info& t = typeid(T);
    return std::static_pointer_cast<ComponentArray<T>>(_componentArrays[t.hash_code()]);
  };

private:
  std::unordered_map<std::size_t, std::shared_ptr<IComponentArray>> _componentArrays{};
};
