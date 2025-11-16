#pragma once

#include "../FacadeStructs.h"
#include "Elements/ContainerElement.cpp"
#include "Elements/Factory/ElementFactory.h"
#include "Observer/EventObserver.h"
#include "Observer/ValueObserver.h"

#include <cstdint>
#include <WestLogger.h>

static std::atomic_uint32_t CURRENT_ID = 1;

class InterfaceBuilder
{
public:
  InterfaceBuilder(EventObserver* eo, ValueObserver* vo);
  InterfaceBuilder();
  ~InterfaceBuilder();

  void createNewInterface(std::uint16_t xScreenPosition,
                          std::uint16_t yScreenPosition,
                          float stretchX,
                          float stretchY,
                          std::uint16_t rows,
                          std::uint16_t columns,
                          bool hiddenContainer);
  void createBackground(TextureInformation* t);
  void addElement(ElementProxy* e);
  ContainerElement* build();
  IElement* transform(ElementProxy* e);

private:
  ContainerElement* _current   = nullptr;
  ElementFactory* _factory     = nullptr;
  ComponentDataPool* _dataPool = nullptr;
  EventObserver* _eObserver    = nullptr;
  ValueObserver* _vObserver    = nullptr;
  WestLogger& _logger          = WestLogger::getLoggerInstance();
};
