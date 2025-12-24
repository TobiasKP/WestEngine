#pragma once

#include "IObserver.h"

class EventObserver : public IObserver
{
public:
  EventObserver(WestLogger& l);
  ~EventObserver() override;

  virtual void handleEvent(std::int16_t elementId,
                           std::uint16_t event,
                           std::uint16_t mouseX,
                           std::uint16_t mouseY,
                           std::string value) override;

  virtual void registerElement(IElement* e) override;
  virtual void deregisterElement(IElement* e) override;

private:
  std::vector<IElement*> _registeredElements;

  void executeElement(IElement* e) override;
  void sort() override;
};
