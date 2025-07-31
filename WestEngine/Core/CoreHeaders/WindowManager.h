#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdint>

#include "Interfaces/IManager.h"

class WindowManager : public IManager {

public:
  WindowManager();
  WindowManager(WestLogger *logger);
  ~WindowManager() override;

  // Getter
  inline GLint getWidth() { return _width; }
  inline GLint getHeight() { return _height; }
  inline std::string etTitle() { return _title; }
  inline GLFWwindow *getWindow() { return _window; }

  // Overrides
  std::int32_t startup() override;
  void shutdown() override;
  void update() override;
  std::int32_t init() override;

  // Functions
  inline bool windowShouldClose() { return glfwWindowShouldClose(_window); }
  void resizeWindow(GLint width, GLint height);
  void setWindowTitle(std::string title);
  void setClearColor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
  }
  bool isKeyPressed(std::int32_t keyCode);
  static void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id,
                                  GLenum severity, GLsizei length,
                                  const GLchar *message, const void *iserParam);

private:
  GLint _width, _height;
  std::string _title = CoreConstants::UNDEFINED_STRING;
  GLFWwindow *_window = nullptr;
};
