#pragma once

#include "IElement.h"

struct DebugCube : public IElement {

  DebugCube() {
    colorR = 1.0;
    colorG = 0;
    colorB = 0;
    colorA = 1.0;
  }

  ~DebugCube() {};

};
