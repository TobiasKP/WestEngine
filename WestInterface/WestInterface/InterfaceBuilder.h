#pragma once

#include "../FacadeStructs.h"
#include "Elements/ContainerElement.hpp"
#include "Elements/Factory/ElementFactory.h"
#include "Observer/EventObserver.h"
#include "Observer/ValueObserver.h"

#include <WestLogger.h>

class InterfaceBuilder
{
public:
  InterfaceBuilder(EventObserver* eo, ValueObserver* vo);
  InterfaceBuilder();
  ~InterfaceBuilder();

  void createNewInterface(Container* c);
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
