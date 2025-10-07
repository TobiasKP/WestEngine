#pragma once

#include "IElement.hpp"

#include <Config.h>
#include <format>
//#include <iostream>

struct Label : public IElement {

  Label() {
    flags = 0;
  }

  void handler() {
    _logger.log(
        Level::Error,
        std::format(
            "@@@ Button handler of interface: {} called which does not exists!",
            this->id));
  };

  void describeMyself(ComponentData *cd, std::uint8_t row, std::uint8_t column) {
    cd->colorR = this->colorR;
    cd->colorG = this->colorG;
    cd->colorB = this->colorB;
    cd->colorA = this->colorA;

    cd->vertices[0] = xLL + (SIZE_E * row);
    cd->vertices[1] = yLL + (SIZE_E * column); 
    cd->flags = flags;
  };
};
