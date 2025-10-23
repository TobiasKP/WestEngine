#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "../Elements/IElement.hpp"

class IObserver {

public:
  IObserver() {};
  virtual ~IObserver() {};

  virtual bool handleEvent(std::uint16_t event, std::uint16_t mouseX,
                           std::uint16_t mouseY, std::string value) {
    return false;
  };

  virtual void registerElement(IElement *e) {};

private:
  std::vector<IElement *> _registeredElements;

  virtual void executeElement(IElement *e) {};
  virtual void sort() {};
};

/**********************************
 *  Events:
 *  0x01: hovered
 *  0x02: unhovered 
 *  0x04: clicked left
 *
 * ********************************/
