#pragma once

#include "EventObserver.h"

EventObserver::EventObserver() {}

EventObserver::~EventObserver() {}

bool EventObserver::handleEvent(std::uint16_t event, std::uint16_t mouseX,
                                std::uint16_t mouseY, std::string value) {
  IElement *e = _registeredElements.at(0);
  if (event & 0x01) {
     e->flags |=  0x02;
    executeElement(e);
  } 
  if(event & 0x02) {
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
