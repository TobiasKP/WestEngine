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
    _interfaceId   = interfaceId;
  }

  void setControlFlag(std::uint8_t flag, float x, float y)
  {
    std::lock_guard<std::mutex> lock(_controlMutex);
    _controlFlags |= flag;
    _x             = x;
    _y             = y;
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
  float _x, _y;
  glm::vec3 _playerDestination;
  PlayerControl* _control;
  WestInterfaceFacade* _facade;
  std::int16_t _interfaceId;
  std::mutex _controlMutex, _generalMutex;
  std::uint8_t _controlFlags{0b0000'0000}, _generalFlags{0b0000'0000};
};
