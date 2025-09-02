#pragma once

#include "../FacadeStructs.h"
#include "Elements/ContainerElement.cpp"
#include "Elements/Factory/ElementFactory.h"

#include <WestLogger.h>
#include <cstdint>

static std::uint8_t _currentId = 1;

class InterfaceBuilder {
public:
  InterfaceBuilder();
  ~InterfaceBuilder();

  void createNewInterface(std::uint16_t xScreenPosition,
                          std::uint16_t yScreenPosition, float scale,
                          std::uint8_t gridCells);
  void addElement(ElementProxy *e);
  ContainerElement *build();
  IElement *transform(ElementProxy *e);

private:
  ContainerElement *_current = nullptr;
  ElementFactory *_factory = nullptr;
  WestLogger &_logger = WestLogger::getLoggerInstance();
};
