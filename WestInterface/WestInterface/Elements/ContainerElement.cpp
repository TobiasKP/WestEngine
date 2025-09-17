#pragma once

#include <algorithm>
#include <assert.h>
#include <format>
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
                              "parent element in x position",
                              e->id));
#endif
      gridPositionX = gridCells;
    }

    if (gridPositionY > gridCells) {
#ifdef DEBUG
      _logger.log(Level::Info,
                  std::format("@@@ Added Element: {} is out of bounds from "
                              "parent element in y position",
                              e->id));
#endif
      gridPositionY = gridCells;
    }

    float elementPosX = xLL + width * gridPositionX;
    float elementPosY = yLL + height * gridPositionY;
    float maxWidth = xLL + width * gridCells;
    float maxHeight = yLL + height * gridCells;
    if (elementPosX + SIZE_E * e->scale > maxWidth ||
        elementPosY + SIZE_E * e->scale > maxHeight) {
      _logger.log(Level::Error, std::format("@@@ Added Element: {} will be to "
                                            "large for parent, not adding...",
                                            e->id));
      return;
    }

    e->xLL = elementPosX;
    e->yLL = elementPosY;
    e->zIndex = 2;
#ifdef DEBUG
    _logger.log(
        Level::Info,
        std::format("@@@ Adding element: {} to children of interface: {}",
                    e->id, this->id));
#endif

    children.push_back(e);
  };

  std::vector<ComponentData *> describeContainer() {
    std::vector<ComponentData *> result;
    for (IElement *element : children) {
      result.push_back(element->describe());
    }

    if (!(flags & isHidden)) {
      result.push_back(this->describe());
    }
    return result;
  }

  void describeMyself(ComponentData *cd) {
    // TODO describe myself
  }

  void handler() {}
};
