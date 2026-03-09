#pragma once

#include "Button.hpp"
#include "IElement.hpp"
#include "Label.hpp"

struct DropDown : public IElement
{
  Label* label   = nullptr;
  Button* button = nullptr;

  DropDown()
  {
    zIndex = 2;
  }

  ~DropDown()
  {
    delete label;
    delete button;  
  }

  void handler() override {

  };

  void describeMyself(ComponentData* cd, std::uint8_t row = 0, std::uint8_t column = 0) override
  {
    assert(button != nullptr && label != nullptr);
    if (column >= label->columnElements)
    {
      button->describeMyself(cd, row, column - label->columnElements);
    }
    else
    {
      label->describeMyself(cd, row, column);
    }
  };

  void getBoundaries(ElementBounds& bounds) override
  {
    button->getBoundaries(bounds);
  }
};
