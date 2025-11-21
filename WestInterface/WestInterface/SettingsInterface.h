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

  Container* createSettingButton();
  IElement* createSettingInterface();
  IElement* createResolutionSetting();
};
