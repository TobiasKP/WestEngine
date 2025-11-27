#pragma once

#include "Button.cpp"
#include "IElement.hpp"
#include "Label.cpp"


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

  void handler()
  {
    if (button != nullptr)
    {
      button->handler();
    }
  };

  void describeMyself(ComponentData* cd, std::uint8_t row = 0, std::uint8_t column = 0)
  {
    if (column == columnElements && button != nullptr)
    {
      button->describeMyself(cd, row, column);
    }
    else if (label != nullptr)
    {
      label->describeMyself(cd, row, column);
    }
    else
    {
      // Fallback: render as basic element
      cd->vertices[1] = yLL + (SIZE_E * row);
      cd->vertices[0] = xLL + (SIZE_E * column);
      cd->stretchX    = stretchX;
      cd->stretchY    = stretchY;
      cd->flags       = flags;
      cd->zIndex      = zIndex;
      cd->colorR      = colorR;
      cd->colorG      = colorG;
      cd->colorB      = colorB;
      cd->colorA      = colorA;
      if (texture > 0)
      {
        cd->texture = texture;
      }
    }
  };
};
