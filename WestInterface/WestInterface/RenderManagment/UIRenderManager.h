#pragma once

#include "../Elements/ContainerElement.hpp"
#include "ComponentData.h"

#include <array>

class UIRenderManager
{
public:
  UIRenderManager() {};
  ~UIRenderManager();

  void updateRenderData(std::array<ContainerElement*, 32>& interfaces, size_t count);
  inline std::vector<ComponentData*>& getRenderData()
  {
    return _data;
  }
  inline const std::vector<ElementBounds*>& getBoundaries() const
  {
    return _boundaryData;
  }
  inline void toggleDirty()
  {
    if (!_dirty)
    {
      _dirty = true;
    }
  }

private:
  WestLogger* _logger = &WestLogger::getLoggerInstance();
  std::vector<ComponentData*> _data;
  std::vector<ElementBounds*> _boundaryData;
  std::mutex _vectorMutex;
  bool _dirty = true;

  void fillComponentData(std::vector<ComponentData*>& cd, std::vector<ElementBounds*>& eb);
  void gatherUIData(std::vector<ComponentData*>& local,
                    const std::array<ContainerElement*, 32>& interfaces,
                    size_t begin,
                    size_t end);
  void gatherBoundaryData(std::vector<ElementBounds*>& local, const std::array<ContainerElement*, 32>& interfaces, size_t begin, size_t end); 
};
