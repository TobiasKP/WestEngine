#include "ValueObserver.h"

void ValueObserver::registerElement(IElement* e)
{
  _registeredElements[e->id] = e;
}

void ValueObserver::deregisterElement(IElement* e)
{
  _registeredElements.erase(e->id);
}

void ValueObserver::handleEvent(
  std::int16_t elementId, std::uint16_t event, std::uint16_t mouseX, std::uint16_t mouseY, std::string value)
{
  assert(elementId > -1);
  if (!_registeredElements.contains(elementId))
  {
    logger.log(
      Level::Info,
      std::format("@@@ --- Warning --- Element with id: {}, does not exist in EventObserver, probably deleted\n",
                  elementId));
    return;
  }
  IElement* e = _registeredElements[elementId];
  if (event & 0x08)
  {
    recalcText(e, value);
    executeElement(e);
  }
  if (event & 0x10)
  {
    setProgress((ProgressBar*)e, value);
    executeElement(e);
  }
};

void ValueObserver::setProgress(ProgressBar* p, std::string& value)
{
  p->progresPerc = stoi(value);
}


void ValueObserver::executeElement(IElement* e)
{
  if (e->parent != nullptr)
  {
    e->parent->changed = true;
  }
  e->changed = true;
};

void ValueObserver::recalcText(IElement* e, std::string& text)
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
