#pragma once

#include "../Interfaces/IComponentArray.hpp"

#include <CoreConstants.hpp>
#include <unordered_map>

template <typename T>
class ComponentArray : public IComponentArray
{
public:
  T* getComponentById(std::uint32_t id)
  {
    if (!idToIdx.contains(id))
    {
      return nullptr;
    }
    std::uint32_t idx = idToIdx[id];

    return &components[idx];
  };
  T* getComponentByIdx(std::uint32_t idx)
  {
    return &components[idx];
  };
  void addComponent(std::uint32_t id, T&& component)
  {
    components[size] = std::move(component);
    idToIdx[id]      = size;
    IdxToId[size]    = id;
    size++;
  };

private:
  std::uint32_t size = 0;
  std::array<T, CoreConstants::MAX_ENTITY_SIZE> components;
  std::unordered_map<std::uint32_t, size_t> idToIdx;
  std::unordered_map<size_t, std::uint32_t> IdxToId;
};
