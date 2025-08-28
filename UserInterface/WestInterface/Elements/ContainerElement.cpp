#pragma once

#include <algorithm>
#include <assert.h>
#include <vector>

#include "IElement.h"

struct ContainerElement : public IElement {
  std::vector<IElement *> children;
  std::uint8_t gridCells = 0;

  ~ContainerElement() {
    for (IElement *e : children) {
      assert(e != nullptr);
      delete e;
    }
    children.clear();
  }

  bool deleteChildById(std::uint32_t d_id) {
    auto result =
        std::find_if(children.begin(), children.end(),
                     [&d_id](IElement *obj) { return obj->id == d_id; });

    if (result == children.end()) {
      // TODO Debug Log
      return false;
    }

    children.erase(result);
    delete *result;
    return true;
  }

  void addChild(IElement *e, std::uint8_t gridPositionX,
                std::uint8_t gridPositionY) {
    float width = scale * SIZE_E;
    float height = scale * SIZE_E;

    if (gridPositionX > gridCells) {
      // TODO debug log
      gridPositionX = gridCells;
    }

    if (gridPositionY > gridCells) {
      // TODO debug log
      gridPositionY = gridCells;
    }

    float elementPosX = xLL + width * gridPositionX;
    float elementPosY = yLL + height * gridPositionY;
    float maxWidth = xLL + width * gridCells;
    float maxHeight = yLL + height * gridCells;
    if (elementPosX + SIZE_E * e->scale > maxWidth ||
        elementPosY + SIZE_E * e->scale > maxHeight) {
      // TODO Log error element too large
      return;
    }

    e->xLL = elementPosX;
    e->yLL = elementPosY;
    e->zIndex = 2;
    // TODO Debug Log

    children.push_back(e);
  };

  void handler() {};
};
