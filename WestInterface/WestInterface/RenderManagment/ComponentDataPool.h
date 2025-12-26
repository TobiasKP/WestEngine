#pragma once

#include "ComponentData.h"

#include <cstdint>
#include <functional>
#include <mutex>
#include <vector>
#include <WestLogger.h>

class ComponentDataPool
{
public:
  ComponentDataPool();
  ~ComponentDataPool();

  // Disable copy constructor and copy assignment to prevent double-free
  ComponentDataPool(const ComponentDataPool&)            = delete;
  ComponentDataPool& operator=(const ComponentDataPool&) = delete;

  // If move semantics are needed, implement them properly later
  ComponentDataPool(ComponentDataPool&&)            = delete;
  ComponentDataPool& operator=(ComponentDataPool&&) = delete;

  std::uint32_t reserveNew(std::uint32_t size);
  bool deleteRange(std::uint32_t start, std::uint32_t end);
  void setPositionUpdateCallback(std::function<void(std::uint32_t oldPos, std::uint32_t newPos)> callback)
  {
    _positionUpdateCallback = callback;
  }

  ComponentData* getDataAtLocation(std::uint32_t location);

private:
  std::uint32_t findFit(std::uint32_t size);
  void defragPool();

  std::function<void(std::uint32_t oldPos, std::uint32_t newPos)> _positionUpdateCallback;
  std::vector<ComponentData*> _poolingData;
  std::vector<bool> _slotUsed;
  std::mutex _poolMutex;
  bool _dirty;
  WestLogger& _logger = WestLogger::getLoggerInstance();
};
