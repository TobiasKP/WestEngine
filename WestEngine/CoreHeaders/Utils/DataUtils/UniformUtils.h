#pragma once

#include <cstdint>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

class UniformUtils
{
public:
  static GLuint createUniform(const char* uniformName, GLuint programId);
  static GLuint createUniformBufferObject(const char* name, size_t size, GLuint bindingPoint);
  static void setUniform(GLuint location, const std::vector<uint32_t>& value);
  static void setUniform(GLuint location, std::int32_t value);
  static void setUniform(GLuint location, std::uint32_t value);
  static void setUniform(GLuint location, bool value);
  static void setUniform(GLuint location, float value);
  static void setUniform(GLuint location, glm::mat4 value);
  static void setUniform(GLuint location, glm::vec3 value);
  static void setUniform(GLuint location, glm::vec4 value);
  static void setUniform(GLuint location, glm::vec2 value);
};
