#include "ElementFactory.h"

#include "../../Observer/EventObserver.h"
#include "../../Observer/ValueObserver.h"

#include "../Button.cpp"
#include "../ContainerElement.cpp"
#include "../Icon.cpp"
#include "../Label.cpp"

IElement *ElementFactory::createElementInternal(ElementProxy *e) {
  ElementType type = e->type;
  IElement *result = nullptr;

  switch (type) {
  case BUTTON:
    //result = new Button();
    fillBasicInfos(e, result);
    registerElementEvent(result);
    break;
  case LABEL:
    //result = new Label();
    fillBasicInfos(e, result);
    break;
  case CONTAINER:
    result = new ContainerElement();
    fillBasicInfos(e, result);
    break;
  case ICON:
    //result = new Icon();
    fillBasicInfos(e, result);
    break;
  default:
    break;
  }

  return result;
}

void ElementFactory::registerElementEvent(IElement *e) {
  EventObserver::registerElement(e);
}

void ElementFactory::registerElementValue(IElement *e) {
  ValueObserver::registerElement(e);
}

void ElementFactory::fillBasicInfos(ElementProxy *ep, IElement *el) {}
