#pragma once

#include "Button.cpp"
#include "ContainerElement.cpp"
#include "IElement.hpp"
#include "Label.cpp"

#include <iostream>

struct DropDown : public IElement
{
  Label* label                = nullptr;
  Button* button              = nullptr; 

  DropDown()
  {
    zIndex = 2;
  }

  ~DropDown()
  {
    delete label;
    delete button;
  }

  void handler() override
  {
    std::cout << "clicked" << std::endl;
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
