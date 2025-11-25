#pragma once

#include "Elements/Umbrella.hpp"
#include "InterfaceBuilder.h"


class SettingsInterface
{
public:
  SettingsInterface(InterfaceBuilder& interfaceBuilder);
  ~SettingsInterface();

  ContainerElement* init();

private:
  InterfaceBuilder* _interfaceBuilder;
  WestLogger& _logger = WestLogger::getLoggerInstance();
  std::vector<ElementProxy*> _elements;
  std::uint8_t _id;

  Container* createSettingButton();
  void createSettingInterface();
  ElementProxy* createResolutionSetting();
  ElementProxy* createQuitSettingsButton();
};
