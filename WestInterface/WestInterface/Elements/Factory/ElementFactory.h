#pragma once

#include "../../../FacadeStructs.h"
#include "../IElement.hpp"

class ElementFactory {

public:
  ElementFactory() {};
  ~ElementFactory() {};

  IElement *createElement(ElementProxy *e) { return createElementInternal(e); };

private:
  IElement *createElementInternal(ElementProxy *e);
  void registerElementEvent(IElement *e);
  void registerElementValue(IElement *e);
  void fillBasicInfos(ElementProxy *ep, IElement *el);
};
