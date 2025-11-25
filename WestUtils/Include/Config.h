#pragma once

#if defined(_WIN32) || defined(_WIN64)
#if defined(WESTUTILS_BUILDING_DLL)
#define WESTUTILS __declspec(dllexport)
#else
#define WESTUTILS __declspec(dllimport)
#endif
#else
#define WESTUTILS __attribute__((visibility("default")))
#endif

#include "ThreadPool.h"

namespace WESTUTILS Config
{

extern WESTUTILS std::atomic<bool> PAUSE;
extern WESTUTILS std::atomic<std::uint32_t> INTERNAL_ENTITY_ID;
extern WESTUTILS std::atomic<std::uint32_t> INTERNAL_UI_ID;
extern WESTUTILS std::atomic_size_t INTERNAL_UI_COUNT;
extern WESTUTILS ThreadPool* THREADPOOL;
extern WESTUTILS std::uint32_t interfaceShaderProgram;
extern WESTUTILS std::uint32_t interfaceOrthoUniform;
extern WESTUTILS std::uint32_t interfaceFontTextureUniform;
extern WESTUTILS std::uint32_t interfaceTextureOneUniform;

struct General
{
  std::uint32_t WIDTH, HEIGHT;
  float SPEED, EPSILON, FPS;
  const std::uint8_t CHUNK_SIZE;
};

extern WESTUTILS General GeneralConfig;

}  // namespace WESTUTILS Config
