#pragma once

#include "../../../FacadeStructs.h"
#include "../../Observer/EventObserver.h"
#include "../../Observer/ValueObserver.h"
#include "../IElement.hpp"
#include "../DropDown.cpp"

#include <array>

class ElementFactory
{
public:
  ElementFactory(EventObserver* eo, ValueObserver* vo)
  {
    _vObserver = vo;
    _eObserver = eo;
  };
  ~ElementFactory() {};

  IElement* createElement(ElementProxy* e)
  {
    return createElementInternal(e);
  };

private:
  IElement* createElementInternal(ElementProxy* e);
  void registerElementEvent(ElementProxy* ep, IElement* e);
  void registerElementValue(ElementProxy* ep, IElement* e);
  void fillBasicInfos(ElementProxy* ep, IElement* el);
  void fillText(ElementProxy* ep, IElement* el);
  void fillTexture(ElementProxy* ep, IElement* el);
  void fillDropdown(ElementProxy* ep, DropDown* d);
  std::array<float, 4> getTextureCoordinatesForChar(char character);

  ValueObserver* _vObserver;
  EventObserver* _eObserver;
};
