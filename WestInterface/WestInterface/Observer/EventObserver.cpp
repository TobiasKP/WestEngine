#include "EventObserver.h"

#include <algorithm>
#include <cassert>
#include <format>

EventObserver::EventObserver(WestLogger& l) : IObserver(l) {}

EventObserver::~EventObserver() {}

void EventObserver::handleEvent(
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

  if (event & 0x01)
  {
    e->flags |= 0x02;
    executeElement(e);
  }
  if (event & 0x02)
  {
    e->flags &= ~0x02;
    executeElement(e);
  }
  if (event & 0x04)
  {
    e->handler();
  }
};

void EventObserver::registerElement(IElement* e)
{
  _registeredElements.push_back(e);
}

void EventObserver::deregisterElement(IElement* e)
{
  auto it = std::find_if(_registeredElements.begin(),
                         _registeredElements.end(),
                         [e](const IElement* element) { return element->id == e->id; });
  if (it != _registeredElements.end())
  {
    _registeredElements.erase(it);
  }
}

void EventObserver::executeElement(IElement* e)
{
  if (e->parent != nullptr)
  {
    e->parent->changed = true;
  }
  e->changed = true;
};

void EventObserver::sort() {};
