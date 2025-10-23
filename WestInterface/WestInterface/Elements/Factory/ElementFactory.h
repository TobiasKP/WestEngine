#pragma once

#include <array>

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
  void fillText(ElementProxy *ep, IElement *el);
  std::array<float, 4> getTextureCoordinatesForChar(char character);  
};
