#pragma once

#include <array>

#include "../Elements/ContainerElement.cpp"
#include "ComponentData.h"

class UIRenderManager {

public:
  UIRenderManager() {};
  ~UIRenderManager();

  void updateRenderData(std::array<ContainerElement *, 32> interfaces,
                        size_t count);
  std::vector<ComponentData *> getRenderData() { return data; }

private:
  WestLogger *_logger = &WestLogger::getLoggerInstance();
  std::vector<ComponentData *> data;
  std::mutex _vectorMutex;

  void fillComponentData(std::vector<ComponentData *> cd);
};
