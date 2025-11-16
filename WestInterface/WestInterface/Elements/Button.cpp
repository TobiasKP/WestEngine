
#pragma once

#include "Label.cpp"

#include <cassert>

struct Button : public Label
{
  std::uint8_t eventId;

  void handler()
  {
    assert(eventHandler != nullptr);
    if (eventHandler)
    {
      eventHandler();
    }
  };

  void describeMyself(ComponentData* cd, std::uint8_t row, std::uint8_t column)
  {
    Label::describeMyself(cd, row, column);
  };
};
