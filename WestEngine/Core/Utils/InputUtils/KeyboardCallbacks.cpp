#include "../../../CoreHeaders/Utils/InputUtils/KeyboardCallbacks.h"

#include "../../../Constants/InternalEvents.hpp"

#include <Config.h>

std::shared_ptr<EventDispatcher> KeyboardCallbacks::_dispatcher;

void KeyboardCallbacks::keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  _dispatcher->dispatchEvent(EventIdentifiers::KEY, KeyboardPayload(key, action));
}


