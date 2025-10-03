#pragma once

#include <algorithm>
#include <assert.h>
#include <format>
#include <iostream>
#include <vector>

#include "IElement.hpp"

struct ContainerElement : public IElement {
  std::vector<IElement *> children;
  std::uint8_t gridCells = 0;

  ContainerElement() {}

  ~ContainerElement() {
#ifdef DEBUG
    _logger.log(
        Level::Info,
        std::format("@@@ Deleting all children for interface: ", this->id));
#endif
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
      _logger.log(Level::Error,
                  std::format("@@@ Trying to delete element: {} from "
                              "interface: {} that does not exists.",
                              d_id, this->id));
      return false;
    }

#ifdef DEBUG
    _logger.log(Level::Info,
                std::format("@@@ Deleting element: {} from interface: {}", d_id,
                            this->id));
#endif
    children.erase(result);
    delete *result;
    return true;
  }

  void addChild(IElement *e, std::uint8_t gridPositionX,
                std::uint8_t gridPositionY) {
    float width = scale * SIZE_E;
    float height = scale * SIZE_E;

    if (gridPositionX > gridCells) {
#ifdef DEBUG
      _logger.log(Level::Info,
                  std::format("@@@ Added Element: {} is out of bounds from "
                              "parent element in x position\n",
                              e->id));
#endif
      gridPositionX = gridCells;
    }

    if (gridPositionY > gridCells) {
#ifdef DEBUG
      _logger.log(Level::Info,
                  std::format("@@@ Added Element: {} is out of bounds from "
                              "parent element in y position\n",
                              e->id));
#endif
      gridPositionY = gridCells;
    }

    float elementPosX = xLL + width * gridPositionX;
    float elementPosY = yLL + height * gridPositionY;
    float maxWidth = xLL + width * gridCells;
    float maxHeight = yLL + height * gridCells;
#ifdef DEBUG
    _logger.log(
        Level::Info,
        std::format("@@@ Element lower Left -> {}:{} - max Size -> {}:{}\n",
                    elementPosX, elementPosY, maxWidth, maxHeight));
#endif
    if (elementPosX + SIZE_E * e->scale > maxWidth ||
        elementPosY + SIZE_E * e->scale > maxHeight) {
      _logger.log(Level::Error, std::format("@@@ Added Element: {} will be to "
                                            "large for parent, not adding...\n",
                                            e->id));
      return;
    }

    e->xLL = elementPosX;
    e->yLL = elementPosY;
    e->zIndex = 2;
#ifdef DEBUG
    _logger.log(
        Level::Info,
        std::format("@@@ Adding element: {} to children of interface: {}\n",
                    e->id, this->id));
#endif

    children.push_back(e);
  };

  std::vector<ComponentData *> describeContainer() {
    std::vector<ComponentData *> result;
    for (IElement *element : children) {
      assert(element != nullptr);
      result.push_back(element->describe());
    }

    if (!(flags & isHidden)) {
      result.push_back(describe());
    }

    return result;
  }

  void describeMyself(ComponentData *cd) {
    // top right
    cd->vertices[0] = xLL + (gridCells * SIZE_E) * scale;
    cd->vertices[1] = yLL + (gridCells * SIZE_E) * scale;
    // bottom right
    cd->vertices[2] = xLL + (gridCells * SIZE_E) * scale;
    cd->vertices[3] = yLL;
    // bottom left
    cd->vertices[4] = xLL;
    cd->vertices[5] = yLL;
    // top left
    cd->vertices[6] = xLL;
    cd->vertices[7] = yLL + (gridCells * SIZE_E) * scale; 
  }

  void handler() {}
};
