#pragma once

#include "Elements/Umbrella.hpp"
#include "InterfaceBuilder.h"

class DebugInfoInterface
{
public:
  DebugInfoInterface(InterfaceBuilder& interfaceBuilder);
  ~DebugInfoInterface();

  ContainerElement* init();
  void refreshInterface();

private:
  WestInterface::Container* createElementInfo(std::vector<WestInterface::ElementProxy*>& elements);

  WestLogger& _logger = WestLogger::getLoggerInstance();
  InterfaceBuilder* _interfaceBuilder; 
};
