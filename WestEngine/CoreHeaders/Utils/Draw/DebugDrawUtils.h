#pragma once

#include "../../Entity/Scene.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class DebugDrawUtils
{
public:
  DebugDrawUtils(WestLogger* logger, std::shared_ptr<Scene> s);
  void unloadModel(const Entity& entity);
  std::uint32_t addLine(glm::vec3 start, glm::vec3 direction);

private:
  std::shared_ptr<Scene> _scene;
};
