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
  void refreshInterface();

private:
  WestLogger& _logger = WestLogger::getLoggerInstance();
  InterfaceBuilder* _interfaceBuilder;
  std::uint8_t _id, _settingButtonId, _resolutionId;
  std::array<std::tuple<std::uint32_t, std::uint32_t>, 4> _supportedResolutions = {
    std::make_tuple(800, 600),
    std::make_tuple(1280, 960),
    std::make_tuple(1920, 1080),
    std::make_tuple(2560, 1440)
  };

  Container* createSettingButton(std::vector<ElementProxy*>& elements);
  void createSettingInterface();
  ElementProxy* createResolutionSetting();
  ElementProxy* createQuitSettingsButton();
  void createResolutionOptions();
  ElementProxy* resolutionOption(std::uint32_t x, std::uint32_t y, std::uint8_t row);
};
