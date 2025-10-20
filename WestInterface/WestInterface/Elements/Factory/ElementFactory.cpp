#include "ElementFactory.h"

#include "../../Observer/EventObserver.h"
#include "../../Observer/ValueObserver.h"
#include "../../RenderManagment/TextRenderManager.h"
#include "../Umbrella.hpp"

#include <unordered_map>

IElement *ElementFactory::createElementInternal(ElementProxy *e) {
  ElementType type = e->type;
  IElement *result = nullptr;

  switch (type) {
  case BUTTON:
    // result = new Button();
    fillBasicInfos(e, result);
    fillText(e, result);
    registerElementEvent(result);
    break;
  case LABEL:
    result = new Label();
    fillBasicInfos(e, result);
    fillText(e, result);
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
    fillText(e, result);
    break;
  default:
    result = new DebugElement();
    fillBasicInfos(e, result);
    break;
  }

  assert(result != nullptr);
  return result;
}

void ElementFactory::fillText(ElementProxy *ep, IElement *el) {
  if (ep->text.size() == 0)
    return;

  Text *t = new Text();
  t->plaintext = ep->text;
  t->coordinates.reserve(ep->text.size() * 8);
  for (char c : ep->text) {
    std::vector<float> coords = getTextureCoordinatesForChar(c);
    t->coordinates.insert(t->coordinates.end(), coords.begin(), coords.end());
  }
  el->text = std::unique_ptr<Text>(t);
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
  el->rowElements = ep->rowElements;
  el->columnElements = ep->columnElements;
}

std::vector<float>
ElementFactory::getTextureCoordinatesForChar(char character) {
  std::unordered_map<char, TextRenderManager::GlyphData> res =
      TextRenderManager::getGlyphCache();

  assert(character >= 32 && character <= 126);
  TextRenderManager::GlyphData *d;
  auto it = res.find(character);
  if (it != res.end()) {
    d = &(it->second);
    return d->textureCoords;
  }

  auto fallback = res.find(' ');
  if (fallback != res.end()) {
    d = &(fallback->second);
    return d->textureCoords;
  }

  return std::vector<float>(0);
}
