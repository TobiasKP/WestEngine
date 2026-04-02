#include "../../CoreHeaders/Systems/CameraSystem.hpp"

#include "../../Constants/UniformConstants.hpp"
#include "../../CoreHeaders/InputManager.h"
#include "../../CoreHeaders/Utils/DataUtils/UniformUtils.h"

CameraSystem::CameraSystem(std::shared_ptr<EventDispatcher> d,
                           WestLogger* l,
                           std::shared_ptr<ComponentRegistry> r,
                           std::shared_ptr<Camera> c)
  : ISystem(d, l, r)
{
  _move = _rot = glm::vec3(0);
  _cam         = c;
  _dirty       = false;
};


CameraSystem::~CameraSystem() {};

void CameraSystem::update()
{
  pollEvents();
  _cam->movePosition(_move.x, _move.y, _move.z);
  _cam->moveRotation(_rot.x, _rot.y, _rot.z);
  _move.y = 0;
};

void CameraSystem::updateDebuggingInfo() {};

void CameraSystem::init(const std::shared_ptr<World>& w)
{
  assert(_cam != nullptr);
  _cam->setCameraUniforms(
    UniformUtils::createUniformBufferObject(UniformConstants::CAMERA_UNIFORMS, sizeof(glm::mat4) * 2, 1));
  _dispatcher->subscribe(EventIdentifiers::MOUSE_WHEEL,
                         [this](EventIdentifiers event, EventPayload payload) { pushEvent(event, payload); });
  _dispatcher->subscribe(EventIdentifiers::KEY,
                         [this](EventIdentifiers event, EventPayload payload) { pushEvent(event, payload); });
};

void CameraSystem::handleEvent(std::tuple<EventIdentifiers, EventPayload> event)
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
      else if (action.compare("CameraRotateRight") == 0)
      {
        _rot.y = -sign;
      }
      else if (action.compare("CameraRotateLeft") == 0)
      {
        _rot.y = sign;
      }
      break;
    }
    default:
      break;
  }
}
