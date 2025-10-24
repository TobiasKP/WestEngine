#pragma once

#include <cassert>

#include "EventObserver.h"

EventObserver::EventObserver() {}

EventObserver::~EventObserver() {}

bool EventObserver::handleEvent(std::int16_t elementId, std::uint16_t event,
                                std::uint16_t mouseX, std::uint16_t mouseY,
                                std::string value) {
  IElement *e = nullptr;
  assert(elementId > -1);
  for (IElement *el : _registeredElements) {
    if (el->id == elementId) {
      e = el;
      break;
    }
  }
  assert(e != nullptr);

  if (event & 0x01) {
    e->flags |= 0x02;
    executeElement(e);
  }
  if (event & 0x02) {
    e->flags &= ~0x02;
    executeElement(e);
  }

  return false;
};

void EventObserver::registerElement(IElement *e) {
  _registeredElements.push_back(e);
}

void EventObserver::executeElement(IElement *e) { e->changed = true; };

void EventObserver::sort() {};
