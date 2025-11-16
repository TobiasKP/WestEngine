#pragma once

#include "IElement.hpp"

#include <Config.h>
#include <format>

struct Label : public IElement
{
  Label()
  {
    flags = 0;
  }

  void handler()
  {
    _logger.log(Level::Error,
                std::format("@@@ Button handler of interface: {} called which does not exists!", this->id));
  };

  void describeMyself(ComponentData* cd, std::uint8_t row, std::uint8_t column)
  {
    cd->colorR = this->colorR;
    cd->colorG = this->colorG;
    cd->colorB = this->colorB;
    cd->colorA = this->colorA;

    cd->vertices[1] = yLL + (SIZE_E * row);

    if (text != nullptr && text->positions.size() > column)
    {
      cd->vertices[0] = xLL + text->positions[column];
    }
    else
    {
      cd->vertices[0] = xLL + (SIZE_E * column);  // Fallback to old method
    }

    cd->stretchX = stretchX;
    cd->stretchY = stretchY;
    cd->flags    = flags;
    cd->zIndex   = zIndex;
    if (text != nullptr && text->coordinates.size() > 4 * column)
    {
      cd->textureCoords[0] = text->coordinates[0 + 4 * column];
      cd->textureCoords[1] = text->coordinates[1 + 4 * column];
      cd->textureCoords[2] = text->coordinates[2 + 4 * column];
      cd->textureCoords[3] = text->coordinates[3 + 4 * column];
    }
  };
};
