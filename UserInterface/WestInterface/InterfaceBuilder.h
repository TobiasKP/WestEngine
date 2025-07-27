#pragma once

#include "../FacadeStructs.h"
#include "Elements/ContainerElement.cpp"

#include <cstdint>

class InterfaceBuilder {
public:
  InterfaceBuilder();
  ~InterfaceBuilder();

  void createNewInterface(std::uint16_t xScreenPosition,
                          std::uint16_t yScreenPosition, float scale,
                          std::uint8_t gridLayout);
  void addElement(ElementProxy *e);
  ContainerElement *build();
  IElement *transform(ElementProxy *e);

private:
  ContainerElement *_current;
  std::uint8_t _currentId;
};
