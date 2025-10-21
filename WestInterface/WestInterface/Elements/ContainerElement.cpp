#pragma once

#include <algorithm>
#include <assert.h>
#include <format>
#include <vector>

#include "IElement.hpp"

struct ContainerElement : public IElement {
  std::vector<IElement *> children;

  ContainerElement() { flags = 0x01; }

  ~ContainerElement() {
#ifdef DEBUG
    _logger.log(
        Level::Info,
        std::format("@@@ Deleting all children for interface: ", this->id));
#endif
    assert(dataPool != nullptr);
    for (IElement *e : children) {
      assert(e != nullptr);
      if (rowElements > 0 && columnElements > 0)
        dataPool->deleteRange(e->poolPosition,
                              e->poolPosition +
                                  e->rowElements * e->columnElements - 1);
      delete e;
    }
    dataPool->deleteRange(poolPosition,
                          poolPosition + rowElements * columnElements - 1);
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
    IElement *elementToDelete = *result;
    assert(dataPool != nullptr);
    if (elementToDelete->rowElements > 0 &&
        elementToDelete->columnElements > 0) {
      dataPool->deleteRange(elementToDelete->poolPosition,
                            elementToDelete->poolPosition +
                                elementToDelete->rowElements *
                                    elementToDelete->columnElements -
                                1);
    }

    children.erase(result);
    return true;
  }

  void addChild(IElement *e, std::uint8_t gridPositionX,
                std::uint8_t gridPositionY) {
    float width = scale * SIZE_E;
    float height = scale * SIZE_E;

    if (gridPositionX > rowElements) {
#ifdef DEBUG
      _logger.log(Level::Info,
                  std::format("@@@ Added Element: {} is out of bounds from "
                              "parent element in x position\n",
                              e->id));
#endif
      gridPositionX = rowElements;
    }

    if (gridPositionY > columnElements) {
#ifdef DEBUG
      _logger.log(Level::Info,
                  std::format("@@@ Added Element: {} is out of bounds from "
                              "parent element in y position\n",
                              e->id));
#endif
      gridPositionY = columnElements;
    }

    float elementPosX = xLL + width * gridPositionX;
    float elementPosY = yLL + height * gridPositionY;
    float maxWidth = xLL + width * rowElements;
    float maxHeight = yLL + height * columnElements;
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
    e->dataPool = dataPool;
    assert(e->dataPool != nullptr);

    children.push_back(e);
  };

  std::vector<ComponentData *> describeContainer() {
    std::vector<ComponentData *> result;
    for (IElement *element : children) {
      assert(element != nullptr && element->rowElements > 0 &&
             element->columnElements > 0);
      for (std::uint8_t i = 0; i < element->rowElements; i++) {
        for (std::uint8_t j = 0; j < element->columnElements; j++) {
          result.push_back(element->describe(i, j));
        }
      }
      element->changed = false;
    }

    assert(rowElements > 0 && columnElements > 0);
    for (std::uint8_t i = 0; i < rowElements; i++) {
      for (std::uint8_t j = 0; j < columnElements; j++) {
        result.push_back(describe(i, j));
      }
    }
    changed = false;

    return result;
  }

  void describeMyself(ComponentData *cd, std::uint8_t row,
                      std::uint8_t column) {
    cd->vertices[0] = xLL + (SIZE_E * row);
    cd->vertices[1] = yLL + (SIZE_E * column);
    cd->flags = flags;
  }

  void updatePositions(std::uint32_t oldPos, std::uint32_t newPos) {
    if (poolPosition == oldPos) {
      poolPosition = newPos;
      return;
    }
    for (IElement *child : children) {
      if (child->poolPosition == oldPos) {
        child->poolPosition = newPos;
        return;
      }
    }
  }

  void handler() {}
};
