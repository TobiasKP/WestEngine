#pragma once

#include "Label.cpp"

struct DebugElement : public Label {

  DebugElement() {
    zIndex = 100;
    flags = 0x00;
  }

  void handler() {
    Label::handler();
  };
  
  void describeMyself(ComponentData* cd) {
    Label::describeMyself(cd);
  };

 
};
