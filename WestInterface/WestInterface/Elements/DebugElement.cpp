#pragma once

#include "Label.cpp"

struct DebugElement : public Label {

  DebugElement() {
    colorR = 1.0;
    colorG = 0;
    colorB = 0;
    colorA = 1.0;
    zIndex = 100;
    flags = 0x00;
  }

  ~DebugElement() {
    Label::~Label();
  };

  void handler() {
    Label::handler();
  };
  
  void describeMyself(ComponentData* cd) {
    Label::describeMyself(cd);
  };

 
};
