#pragma once

#include "ThreadPool.h"

#include <atomic>

namespace Config
{

extern std::atomic<bool> PAUSE;
extern std::atomic_size_t INTERNAL_UI_COUNT;
extern ThreadPool* THREADPOOL;
extern std::int32_t interfaceShaderProgram;
extern std::int32_t interfaceOrthoUniform;
extern std::int32_t interfaceFontTextureUniform;
extern std::int32_t interfaceTextureOneUniform;
extern std::int32_t requestedWidth;
extern std::int32_t requestedHeight;

struct General
{
  FILE* SETTINGS;
  std::uint32_t WIDTH, HEIGHT;
  float SPEED, EPSILON, FPS, DELTA;
  const std::uint8_t CHUNK_SIZE;
  const std::uint8_t THREAD_COUNT;
};

struct Info
{
  std::int32_t TOTAL_ENTITIES, CULLED_ENTITIES;
};

struct Interfaces
{
  std::atomic<std::int32_t> TE_ID, CE_ID, INFO_ID, SETTING_ID;
};


extern std::uint32_t incEntityId();
extern std::uint32_t incUiId();

extern General GeneralConfig;
extern Info GeneralInfo;
extern Interfaces GeneralInterfaces;

}  // namespace Config
