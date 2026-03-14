#pragma once

#include "../Elements/IElement.hpp"

#include <cstdint>
#include <string>

class IObserver
{
public:
  IObserver(WestLogger& l) : logger(l) {};
  virtual ~IObserver() {};

  virtual void handleEvent(
    std::int16_t elementId, std::uint16_t event, std::uint16_t mouseX, std::uint16_t mouseY, std::string value) {};
  virtual void registerElement(IElement* e) {};
  virtual void deregisterElement(IElement* e) {};

protected:
  WestLogger& logger;

private: 
  virtual void executeElement(IElement* e) {};
  virtual void sort() {};
};

/**********************************
 *  Events:
 *  0x01: hovered
 *  0x02: unhovered
 *  0x04: clicked left
 *  0x08: value changed
 *  0x10: progress changed
 * ********************************/
