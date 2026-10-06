#pragma once

#include "../Core/Events/EventDispatcher.hpp"
#include "Interfaces/IManager.h"

#include <cstdint>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <lua.hpp>

class WindowManager : public IManager
{
public:
  WindowManager();
  WindowManager(WestLogger* logger, const std::shared_ptr<EventDispatcher>& d);
  ~WindowManager() override;

  // Getter
  inline GLint getWidth()
  {
    return _width;
  }
  inline GLint getHeight()
  {
    return _height;
  }
  inline GLFWwindow* getWindow()
  {
    return _window;
  }

  // Overrides
  std::int32_t startup() override;
  void shutdown() override;
  void update() override;
  std::int32_t init() override;

  // Functions
  void resizeWindow(GLint width, GLint height);
  void setWindowTitle(const std::string& title);
  void setClearColor(float r, float g, float b, float a)
  {
    glClearColor(r, g, b, a);
  }
  bool isKeyPressed(std::int32_t keyCode);
  static void GLAPIENTRY messageCallback(
    GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* me);
  static void errorCallback(std::int32_t error, const char* message);
  static int getResolution(lua_State* L);

private:
  GLint _width, _height;
  GLFWwindow* _window = nullptr;
  std::shared_ptr<EventDispatcher> _dispatcher;
  std::int32_t _frames   = 0;
  double _frameStartTime = 0;
};
