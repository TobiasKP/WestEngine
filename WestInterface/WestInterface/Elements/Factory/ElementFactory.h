#pragma once

#include "../../../FacadeStructs.h"
#include "../../Observer/EventObserver.h"
#include "../../Observer/ValueObserver.h"
#include "../DropDown.hpp"
#include "../IElement.hpp"

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

  IElement* createElement(WestInterface::ElementProxy* e)
  {
    return createElementInternal(e);
  };

private:
  IElement* createElementInternal(WestInterface::ElementProxy* e);
  void registerElementEvent(WestInterface::ElementProxy* ep, IElement* e);
  void registerElementValue(WestInterface::ElementProxy* ep, IElement* e);
  void fillBasicInfos(WestInterface::ElementProxy* ep, IElement* el);
  void fillText(WestInterface::ElementProxy* ep, IElement* el);
  void fillTexture(WestInterface::ElementProxy* ep, IElement* el);
  void fillDropdown(WestInterface::ElementProxy* ep, DropDown* d);
  std::array<float, 4> getTextureCoordinatesForChar(char character);

  ValueObserver* _vObserver;
  EventObserver* _eObserver;
};
