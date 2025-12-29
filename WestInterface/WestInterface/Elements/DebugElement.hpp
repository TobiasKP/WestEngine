#pragma once

#include "Label.hpp"

struct DebugElement : public Label
{
  DebugElement() : Label()
  {
    zIndex = 100;
  }

  void handler()
  {
    Label::handler();
  };

  void describeMyself(ComponentData* cd, std::uint8_t row, std::uint8_t column)
  {
    Label::describeMyself(cd, row, column);
  };
};
