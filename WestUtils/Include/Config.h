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

namespace WESTUTILS Config {

inline std::atomic<bool> PAUSE = false;
inline std::atomic<std::uint32_t> INTERNAL_ENTITY_ID = 900000;
inline ThreadPool *THREADPOOL = new ThreadPool(4);
inline std::uint32_t interfaceShaderProgram = -1;
inline std::uint32_t interfaceOrthoUniform = -1;

inline struct General {
  std::uint32_t WIDTH = 800, HEIGHT = 600;
  float SPEED = 0.05f, EPSILON = 1e-6f, FPS = 60.0f;
  const std::uint8_t CHUNK_SIZE = 64;
} GeneralConfig;

} // namespace WESTUTILS Config
