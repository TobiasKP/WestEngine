#pragma once

#include "IObserver.h"

#include "../Elements/ProgressBar.hpp"

class ValueObserver : public IObserver
{
public:
  ValueObserver(WestLogger& l) : IObserver(l) {};
  ~ValueObserver() {};

  virtual void handleEvent(std::int16_t elementId,
                           std::uint16_t event,
                           std::uint16_t mouseX,
                           std::uint16_t mouseY,
                           std::string value) override;
  virtual void registerElement(IElement* e) override;
  virtual void deregisterElement(IElement* e) override;

private:
  std::unordered_map<std::int32_t, IElement*> _registeredElements;

  void executeElement(IElement* e) override;
  void recalcText(IElement* e, std::string& text);
  void setProgress(ProgressBar* p, std::string& value);
};
