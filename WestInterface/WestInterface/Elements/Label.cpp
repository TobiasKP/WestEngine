#pragma once

#include "IElement.hpp"

#include <Config.h>
#include <format>

struct Label : public IElement {
  void handler() {
    _logger.log(
        Level::Error,
        std::format(
            "@@@ Button handler of interface: {} called which does not exists!",
            this->id));
  };

  void describeMyself(ComponentData *cd) {
    cd->colorR = this->colorR;
    cd->colorG = this->colorG;
    cd->colorB = this->colorB;
    cd->colorA = this->colorA;
    // top right
    cd->vertices[0] = xLL + SIZE_E;
    cd->vertices[1] = yLL + SIZE_E;
    // bottom right
    cd->vertices[2] = xLL + SIZE_E;
    cd->vertices[3] = yLL;
    // bottom left
    cd->vertices[4] = xLL;
    cd->vertices[5] = yLL;
    // top left
    cd->vertices[6] = xLL;
    cd->vertices[7] = yLL + SIZE_E;
  };

  ~Label() {};
};
