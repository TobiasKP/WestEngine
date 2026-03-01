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
    if (!_idToIdx.contains(id))
    {
      return nullptr;
    }
    std::uint32_t idx = _idToIdx[id];

    return &_components[idx];
  };

  T* getComponentByIdx(std::uint32_t idx)
  {
    return &_components[idx];
  };

  std::uint32_t getEntityIdByIdx(size_t idx)
  {
    if (!_idxToId.contains(idx))
    {
      return UINT32_MAX;
    }
    return _idxToId[idx];
  };

  void addComponent(std::uint32_t id, T&& component)
  {
    _components[_size] = std::move(component);
    _idToIdx[id]       = _size;
    _idxToId[_size]    = id;
    _size++;
  };

  bool remove(std::uint32_t id)
  {
    if (!_idToIdx.contains(id))
    {
      return false;
    }
    std::uint32_t idx          = _idToIdx[id];
    std::uint32_t lastIdx      = _size - 1;
    std::uint32_t lastEntityId = _idxToId[lastIdx];
    _components[idx]           = std::move(_components[lastIdx]);
    _size--;
    _idToIdx[lastEntityId] = idx;
    _idxToId[idx]          = lastEntityId;
    _idToIdx.erase(id);
    _idxToId.erase(lastIdx);
    return true;
  }

  std::array<T, CoreConstants::MAX_ENTITY_SIZE>& getComponents()
  {
    return _components;
  }

  size_t getSize()
  {
    return _size;
  }

private:
  std::uint32_t _size = 0;
  std::array<T, CoreConstants::MAX_ENTITY_SIZE> _components;
  std::unordered_map<std::uint32_t, size_t> _idToIdx;
  std::unordered_map<size_t, std::uint32_t> _idxToId;
};
