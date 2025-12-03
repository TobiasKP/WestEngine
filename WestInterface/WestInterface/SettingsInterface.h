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
  std::uint8_t _id, _resolutionId;

  Container* createSettingButton();
  void createSettingInterface();
  ElementProxy* createResolutionSetting();
  ElementProxy* createQuitSettingsButton();
  void createResolutionOptions(); 
};
