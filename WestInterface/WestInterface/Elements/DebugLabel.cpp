#pragma once

#include "Label.cpp"

struct DebugLabel : public Label {


  DebugLabel() {
    colorR = 1.0;
    colorG = 0;
    colorB = 0;
    colorA = 1.0;
    zIndex = 100; 
    flags = 0x00;  
  }

  ~DebugLabel() {};

  void handler() {};
};
