#include "ValueObserver.h"

void ValueObserver::registerElement(IElement* e)
{
  _registeredElements.push_back(e);
}

void ValueObserver::deregisterElement(IElement* e)
{
  auto it = std::find_if(_registeredElements.begin(),
                         _registeredElements.end(),
                         [e](const IElement* element) { return element->id == e->id; });
  if (it != _registeredElements.end())
  {
    _registeredElements.erase(it);
  }
}

void ValueObserver::handleEvent(
  std::int16_t elementId, std::uint16_t event, std::uint16_t mouseX, std::uint16_t mouseY, std::string value)
{
  IElement* e = nullptr;
  assert(elementId > -1);
  for (IElement* el : _registeredElements)
  {
    if (el->id == elementId)
    {
      e = el;
      break;
    }
  }

  if (e == nullptr)
  {
    logger.log(
      Level::Info,
      std::format("@@@ --- Warning --- Element with id: {}, does not exist in EventObserver, probably deleted\n",
                  elementId));
    return;
  }

  if (event & 0x08)
  {
    recalcText(e, value);
    executeElement(e);
  }
};


void ValueObserver::executeElement(IElement* e)
{
  if (e->parent != nullptr)
  {
    e->parent->changed = true;
  }
  e->changed = true;
};

void ValueObserver::recalcText(IElement* e, std::string text)
{
  Text* t      = new Text();
  t->plaintext = text;

  t->coordinates.reserve(text.size() * 4);
  t->positions.reserve(text.size());
  t->positions = TextRenderManager::calculateTextPositions(text, e->stretchX);

  for (char c : text)
  {
    std::array<float, 4> coords = TextRenderManager::getTextureCoordinatesForChar(c);
    t->coordinates.insert(t->coordinates.end(), coords.begin(), coords.end());
  }

  assert(t->coordinates.size() == text.size() * 4);
  assert(t->positions.size() == text.size());
  e->text = std::unique_ptr<Text>(t);
}
