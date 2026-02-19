#pragma once

#include "../../../CoreHeaders/Systems/Umbrella.h"

#include <cstdint>
#include <WestInterfaceFacade.h>

class InputObserver
{
public:
  InputObserver();
  ~InputObserver() {};

  void notify();

  void setControlFlag(std::uint8_t flag)
  {
    std::lock_guard<std::mutex> lock(_controlMutex);
    _controlFlags |= flag;
  }

  void setControlFlag(std::uint8_t flag, std::uint16_t interfaceId)
  {
    std::lock_guard<std::mutex> lock(_controlMutex);
    _controlFlags |= flag;
    if (flag & BitMasks::Control::UI_UNHOVERED)
    {
      _interfaceUnhoverId = interfaceId;
    }
    else
    {
      _interfaceHoverId = interfaceId;
    }
  }

  void setControlFlag(std::uint8_t flag, float x, float z)
  {
    std::lock_guard<std::mutex> lock(_controlMutex);
    _controlFlags |= flag;
    _x             = x;
    _z             = z;
  }

  void setControlFlag(std::uint8_t flag, glm::vec3 dest)
  {
    std::lock_guard<std::mutex> lock(_controlMutex);
    _controlFlags      |= flag;
    _playerDestination  = dest;
  }

  void setGeneralFlag(std::uint8_t flag)
  {
    std::lock_guard<std::mutex> lock(_generalMutex);
    _generalFlags |= flag;
  }

private:
  float _x, _z;
  glm::vec3 _playerDestination;
  PlayerControl* _control;
  WestInterface::WestInterfaceFacade* _facade;
  std::int16_t _interfaceHoverId, _interfaceUnhoverId = -1;
  std::mutex _controlMutex, _generalMutex;
  std::uint8_t _controlFlags{0b0000'0000}, _generalFlags{0b0000'0000};
};
