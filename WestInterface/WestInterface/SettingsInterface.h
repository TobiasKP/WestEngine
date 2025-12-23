#pragma once

#include "Elements/Umbrella.hpp"
#include "InterfaceBuilder.h"

#include <array>

class SettingsInterface
{
public:
  SettingsInterface(InterfaceBuilder& interfaceBuilder);
  ~SettingsInterface();

  ContainerElement* init();

private:
  WestLogger& _logger = WestLogger::getLoggerInstance();
  InterfaceBuilder* _interfaceBuilder;
  std::vector<ElementProxy*> _elements;
  std::uint8_t _id, _resolutionId;
  std::array<std::tuple<std::uint32_t, std::uint32_t>, 2> _supportedResolutions = {
    std::make_tuple(800, 600),
    std::make_tuple(1280, 960),
  };

  Container* createSettingButton();
  void createSettingInterface();
  ElementProxy* createResolutionSetting();
  ElementProxy* createQuitSettingsButton();
  void createResolutionOptions();
  ElementProxy* resolutionOption(std::uint32_t x, std::uint32_t y);
};
