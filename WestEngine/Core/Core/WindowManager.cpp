#include "../CoreHeaders/WindowManager.h"

#include <../../../Libs/GLM/ext/matrix_clip_space.hpp>
#include <../../../Libs/GLM/glm.hpp>

#include "../Constants/CoreConstants.h"
#include "../CoreHeaders/Utils/InputUtils/KeyboardCallbacks.h"
#include "../CoreHeaders/Utils/InputUtils/MouseCallbacks.h"
#include "../CoreHeaders/Utils/TimeUtils.h"
#include "../Config/Config.h"

WindowManager::WindowManager() : IManager(nullptr) {
  setName(CoreConstants::WINDOW_MANAGER);
  _width = 0;
  _height = 0;
  _title = "";
}

WindowManager::WindowManager(WestLogger *logger) : IManager(logger) {
  setName(CoreConstants::WINDOW_MANAGER);
  _width = Config::GeneralConfig.WIDTH;
  _height = Config::GeneralConfig.HEIGHT;
  _title = CoreConstants::TITLE;
  assert(_width > 0 && _height > 0);
}

WindowManager::~WindowManager() {}

std::int32_t WindowManager::startup() {
  if (!glfwInit()) {
    logFailure("GLFW Init failed\n");
    glfwTerminate();
    return 1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  _window = glfwCreateWindow(_width, _height, _title, NULL, NULL);

  if (!_window) {
    logFailure("Window creation failed\n");
    glfwTerminate();
    return 1;
  }

  std::int32_t bufferWidth, bufferHeight;
  glfwGetFramebufferSize(_window, &bufferWidth, &bufferHeight);
  glfwMakeContextCurrent(_window);

  if (glewInit() != GLEW_OK) {
    logFailure("GLEW init failed\n");
    glfwTerminate();
    return 1;
  }

  glViewport(0, 0, bufferWidth, bufferHeight);

  glClearColor(1.0f, 1.0f, 0.0f, 0.5f);
#ifdef DEBUG
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(this->MessageCallback, this);
#endif
  return 0;
}

void WindowManager::shutdown() {
#ifdef DEBUG
  getString()->format("%s ### Shutting down %s...\n", getName(), getName());
  logDebug(getString()->getBuffer());
#endif
  assert(_window);
  glfwDestroyWindow(_window);
  glfwTerminate();
}

std::int32_t WindowManager::init() {

#ifdef DEBUG
  double start = TimeUtils::getCurrentTimeAsTime();
#endif

  KeyboardCallbacks::setWindowManager(this);
  glfwSetKeyCallback(_window, KeyboardCallbacks::keyboardCallback);
  glfwSetCursorPosCallback(_window, MouseCallbacks::mouseCallback);
  glfwSetCursorEnterCallback(_window, MouseCallbacks::enterCallback);
  glfwSetMouseButtonCallback(_window, MouseCallbacks::mouseButtonCallback);

#ifdef DEBUG
  getString()->format("%s: initialized with \n\t\tWidth: %d\n\t\tHeight: %d\n",
                      getName(), getWidth(), getHeight());
  logDebug(getString()->getBuffer());
  double end = TimeUtils::getCurrentTimeAsTime();
  double res = TimeUtils::getDuration(start, end);
  getString()->format("%s ### WindowManager init time: %f ms.\n", getName(),
                      res);
  logDebug(getString()->getBuffer());
#endif

  return 0;
}

void WindowManager::update() {
  glfwSwapBuffers(_window);
  glfwPollEvents();
}

void WindowManager::MessageCallback(GLenum source, GLenum type, GLuint id,
                                    GLenum severity, GLsizei length,
                                    const GLchar *message,
                                    const void *userParam) {
  WindowManager *instance =
      static_cast<WindowManager *>(const_cast<void *>(userParam));
  instance->getString()->format(
      "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
      (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity,
      message);
  instance->logFailure(instance->getString()->getBuffer());
}

void WindowManager::resizeWindow(GLint width, GLint height) {
  assert(width > 0 && height > 0);
  glfwSetWindowSize(_window, width, height);
  //TODO set Ortho
  //Global::UserInterface::ORTHO_MATRIX =
  //    glm::ortho(0.0f, (float)width, 0.0f, (float)height);
  Config::GeneralConfig.HEIGHT = height;
  Config::GeneralConfig.WIDTH = width;
}

void WindowManager::setWindowTitle(const char *title) {
  assert(title != nullptr);
  glfwSetWindowTitle(_window, title);
}

bool WindowManager::isKeyPressed(std::int32_t keyCode) {
  assert(keyCode > -1);
  return glfwGetKey(_window, keyCode) == GLFW_PRESS;
}
