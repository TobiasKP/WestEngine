#pragma once

#include <cstdint>
#include <vector>
#include <string>

#include "../Elements/IElement.hpp"

class IObserver {

public:
  IObserver() {};
  virtual ~IObserver() {};

  virtual void addElement(IElement *e) {};
  virtual bool handleEvent(std::uint8_t event, std::uint16_t mouseX,
                           std::uint16_t mouseY, std::string value) {};

  static void registerElement(IElement *e) {};

private:
  static std::vector<IElement *> registeredElements;
 
  virtual void executeElement(IElement *e) {};
  virtual void sort() {};

};
