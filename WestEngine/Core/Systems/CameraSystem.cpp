#include "../../CoreHeaders/Systems/CameraSystem.hpp"

#include "../../CoreHeaders/Entity/Scene.h"
#include "../../CoreHeaders/InputManager.h"

CameraSystem::CameraSystem(WestLogger* logger, std::shared_ptr<EventDispatcher> d)
{
  _move       = glm::vec3(0);
  _cam        = nullptr;
  _dirty      = false;
  _logger     = logger;
  _dispatcher = d;
};


CameraSystem::~CameraSystem() {};

void CameraSystem::update()
{
  pollEvents();
  _cam->movePosition(_move.x, _move.y, _move.z);
  _move.y = 0;
};

void CameraSystem::updateDebuggingInfo() {};

void CameraSystem::init()
{
  _cam = Scene::getSceneInstance().getCamera();
  assert(_cam != nullptr);
  _dispatcher->subscribe(EventIdentifiers::MOUSE_WHEEL,
                         [this](EventIdentifiers event, EventPayload payload) { pushEvent(event, payload); });
  _dispatcher->subscribe(EventIdentifiers::KEY,
                         [this](EventIdentifiers event, EventPayload payload) { pushEvent(event, payload); });
};

void CameraSystem::pollEvents()
{
  std::vector<std::tuple<EventIdentifiers, EventPayload>> events = _eventQueue.drain();
  for (std::tuple<EventIdentifiers, EventPayload> event : events)
  {
    switch (std::get<0>(event))
    {
      case EventIdentifiers::MOUSE_WHEEL:
      {
        MousePayload* p = std::get_if<MousePayload>(&std::get<1>(event));
        _move.y         = p->scroll;
        break;
      }
      case EventIdentifiers::KEY:
      {
        KeyboardPayload* k = std::get_if<KeyboardPayload>(&std::get<1>(event));
        if (!InputManager::getInputMap().contains(k->key))
        {
          break;
        }
        std::string action = InputManager::getInputMap()[k->key];
        float sign         = (k->action == GLFW_PRESS || k->action == GLFW_REPEAT) ? 1.0f : 0.0f;
        if (action.compare("CameraUp") == 0)
        {
          _move.z = -sign;
        }
        else if (action.compare("CameraDown") == 0)
        {
          _move.z = sign;
        }
        else if (action.compare("CameraLeft") == 0)
        {
          _move.x = -sign;
        }
        else if (action.compare("CameraRight") == 0)
        {
          _move.x = sign;
        }
        break;
      }
      default:
        break;
    }
  }
};
