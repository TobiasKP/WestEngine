#include "EventObserver.h"

#include <cassert>
#include <format>

EventObserver::EventObserver(WestLogger& l) : IObserver(l) {}

EventObserver::~EventObserver() {}

void EventObserver::handleEvent(
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
  _registeredElements[e->id] = e;
}

void EventObserver::deregisterElement(IElement* e)
{
  _registeredElements.erase(e->id);
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
