#pragma once

#include <cstdint>

#include "../RenderManagment/ComponentData.h"

struct Text {};

struct IElement {
  std::uint32_t id;
  
  float xStart;
  float yStart;
  float scale;
  float uvTopLeft;
  float uvBottomRight;
  float colorR;
  float colorG;
  float colorB;
  float colorA;

  virtual void describeSpecificData(ComponentData cd) {};
  ComponentData describe() {
    ComponentData cd;
    describeSpecificData(cd);
    return cd;
  };
  virtual void handler();
};
