
#pragma once

#include "Label.hpp"

#include <cassert>

struct Button : public Label
{
  std::uint8_t eventId;

  void handler() override
  {
    assert(eventHandler != nullptr);
    if (eventHandler)
    {
      eventHandler();
    }
  };

  void describeMyself(ComponentData* cd, std::uint8_t row, std::uint8_t column) override
  {
    Label::describeMyself(cd, row, column);
  };

};
