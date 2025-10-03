#include "ElementFactory.h"

#include "../../Observer/EventObserver.h"
#include "../../Observer/ValueObserver.h"
#include "../Umbrella.hpp"

IElement *ElementFactory::createElementInternal(ElementProxy *e) {
  ElementType type = e->type;
  IElement *result = nullptr;

  switch (type) {
  case BUTTON:
    // result = new Button();
    fillBasicInfos(e, result);
    registerElementEvent(result);
    break;
  case LABEL:
    result = new Label();
    fillBasicInfos(e, result);
    break;
  case CONTAINER:
    result = new ContainerElement();
    fillBasicInfos(e, result);
    break;
  case ICON:
    // result = new Icon();
    fillBasicInfos(e, result);
    break;
  case DEBUG_ELEMENT:
    result = new DebugElement();
    fillBasicInfos(e, result);
    break;
  default:
    result = new DebugElement();
    fillBasicInfos(e, result);
    break;
  }

  assert(result != nullptr);
  return result;
}

void ElementFactory::registerElementEvent(IElement *e) {
  EventObserver::registerElement(e);
}

void ElementFactory::registerElementValue(IElement *e) {
  ValueObserver::registerElement(e);
}

void ElementFactory::fillBasicInfos(ElementProxy *ep, IElement *el) {
  el->id = ep->elementId;
  el->xLL = ep->xPosition;
  el->yLL = ep->yPosition;
  el->scale = ep->scale;
  el->colorR = ep->colorR;
  el->colorG = ep->colorG;
  el->colorB = ep->colorB;
  el->colorA = ep->colorA;
}
