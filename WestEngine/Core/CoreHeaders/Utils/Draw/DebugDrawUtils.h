#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include "../../Entity/Scene.h"
#include "../DataUtils/ObjectLoader.h"

class DebugDrawUtils {
public:
  DebugDrawUtils(WestLogger *logger);
  void unloadModel(Entity *entity);
  Entity *addLine(glm::vec3 start, glm::vec3 direction, glm::vec3 color);

private:
  ObjectLoader *_loader;
  Scene *_scene;
};
