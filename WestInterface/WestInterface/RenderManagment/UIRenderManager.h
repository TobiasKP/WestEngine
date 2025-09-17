#pragma once

#include "ComponentData.h"

class UIRenderManager {

public:
  UIRenderManager();
  ~UIRenderManager();

  void updateRenderData();
  ComponentData *transformRenderData();
};
