#pragma once

#include <array>

#include "../Elements/ContainerElement.cpp"
#include "ComponentData.h"

class UIRenderManager {

public:
  UIRenderManager();
  ~UIRenderManager();

  void updateRenderData(std::array<ContainerElement *, 32> interfaces);
  std::vector<ComponentData *> getRenderData() { return data; }

private:
  std::vector<ComponentData *> data;
};
