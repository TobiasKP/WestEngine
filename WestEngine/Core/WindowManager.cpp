#include "../CoreHeaders/WindowManager.h"

#include "../CoreHeaders/Utils/InputUtils/KeyboardCallbacks.h"
#include "../CoreHeaders/Utils/InputUtils/MouseCallbacks.h"

#include <Config.h>
#include <format>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/glm.hpp>
#include <CoreConstants.hpp>

WindowManager::WindowManager() : IManager(nullptr)
{
  setName(CoreConstants::WINDOW_MANAGER);
  _width  = 0;
  _height = 0;
}

WindowManager::WindowManager(WestLogger* logger) : IManager(logger)
{
  setName(CoreConstants::WINDOW_MANAGER);
  _width  = 0;
  _height = 0;
}

WindowManager::~WindowManager() {}

std::int32_t WindowManager::startup()
{
  _width  = Config::GeneralConfig.WIDTH;
  _height = Config::GeneralConfig.HEIGHT;
  assert(_width > 0 && _height > 0);

  if (!glfwInit())
  {
    logFailure("GLFW Init failed\n");
    glfwTerminate();
    return 1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_MAXIMIZED, GL_FALSE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  std::int32_t monitorPosX, monitorPosY;
  GLFWmonitor* primary    = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(primary);
  glfwGetMonitorPos(primary, &monitorPosX, &monitorPosY);
  if (glfwGetError(NULL) != GLFW_NO_ERROR)
  {
    logFailure(std::format("{} ### Error setting glfw specific informations, stopping startup.\n", getName()));
    glfwTerminate();
    return 1;
  }

  logDebug(std::format("{} ### primary monitor resolution: {} x {}, setting to {} x {}\n",
                       getName(),
                       mode->width,
                       mode->height,
                       _width,
                       _height));

  _window = glfwCreateWindow(_width, _height, CoreConstants::TITLE.data(), NULL, NULL);

  if (!_window)
  {
    logFailure("Window creation failed\n");
    glfwTerminate();
    return 1;
  }

  std::int32_t bufferWidth, bufferHeight;
  std::int32_t windowPosX = monitorPosX + (mode->width - _width) / 2,
               windowPosY = monitorPosY + (mode->height - _height) / 2;
  glfwShowWindow(_window);
  glfwSetWindowSize(_window, _width, _height);
  glfwSetWindowPos(_window, windowPosX, windowPosY);
  glfwGetFramebufferSize(_window, &bufferWidth, &bufferHeight);
  glfwMakeContextCurrent(_window);

  if (glewInit() != GLEW_OK)
  {
    logFailure("GLEW init failed\n");
    glfwTerminate();
    return 1;
  }
  else if (glfwGetError(NULL) != GLFW_NO_ERROR)
  {
    logFailure(std::format("{} ### Error setting window specific informations, "
                           "stopping startup.\n",
                           getName()));
    glfwTerminate();
    return 1;
  }

  glViewport(0, 0, bufferWidth, bufferHeight);

  glClearColor(1.0f, 1.0f, 0.0f, 0.5f);
#ifdef DEBUG
  logDebug(std::format("{} ### Instantiating GL debug callback\n", getName()));
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(this->messageCallback, this);
  logDebug(std::format("{} ### Instantiating GL error callback\n", getName()));
#endif
  glfwSetErrorCallback(errorCallback);

  return 0;
}

void WindowManager::shutdown()
{
#ifdef DEBUG
  logDebug(std::format("{} ### Shutting down {}...\n", getName(), getName()));
#endif
  assert(_window);
  glfwDestroyWindow(_window);
  glfwTerminate();
}

std::int32_t WindowManager::init()
{
#ifdef DEBUG
  double start = TimeUtils::getCurrentTimeAsTime();
#endif

  KeyboardCallbacks::setWindowManager(this);
  glfwSetKeyCallback(_window, KeyboardCallbacks::keyboardCallback);
  glfwSetCursorPosCallback(_window, MouseCallbacks::mouseCallback);
  glfwSetCursorEnterCallback(_window, MouseCallbacks::enterCallback);
  glfwSetMouseButtonCallback(_window, MouseCallbacks::mouseButtonCallback);

#ifdef DEBUG
  logDebug(std::format("{}: initialized with \n\t\tWidth: {}\n\t\tHeight: {}\n", getName(), getWidth(), getHeight()));
  double end = TimeUtils::getCurrentTimeAsTime();
  double res = TimeUtils::getDuration(start, end);
  logDebug(std::format("{} ### WindowManager init time: {} ms.\n", getName(), res));
#endif

  return 0;
}

void WindowManager::update()
{
  glfwSwapBuffers(_window);
  glfwPollEvents();
}

void WindowManager::errorCallback(std::int32_t error, const char* message)
{
  WestLogger::getLoggerInstance().log(Level::Error, std::format("$$$ {} thrown: {}", error, message));
}

void WindowManager::messageCallback(
  GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* me)
{
  WestLogger::getLoggerInstance().log(Level::Info,
                                      std::format("$$$ GL CALLBACK: {} type = 0x{}, severity = 0x{}, message = {}\n",
                                                  (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
                                                  type,
                                                  severity,
                                                  message));
}

void WindowManager::resizeWindow(GLint width, GLint height)
{
  assert(width > 0 && height > 0);
#ifdef DEBUG
  logDebug(std::format("{} ### Resizing window to new height: {}, {}\n", getName(), width, height));
#endif
  glfwSetWindowSize(_window, width, height);
  glViewport(0, 0, width, height);

  Config::GeneralConfig.HEIGHT = height;
  Config::GeneralConfig.WIDTH  = width;
}

void WindowManager::setWindowTitle(const std::string& title)
{
  glfwSetWindowTitle(_window, title.c_str());
}

bool WindowManager::isKeyPressed(std::int32_t keyCode)
{
  assert(keyCode > -1);
  return glfwGetKey(_window, keyCode) == GLFW_PRESS;
}
