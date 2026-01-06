#include "../Include/Config.h"

#include <cassert>

namespace Config
{

std::atomic<bool> PAUSE                       = false;
std::atomic<std::uint32_t> INTERNAL_ENTITY_ID = 1;
std::atomic<std::uint32_t> INTERNAL_UI_ID     = 1;
std::atomic_size_t INTERNAL_UI_COUNT          = 0;
ThreadPool* THREADPOOL                        = new ThreadPool();
std::int32_t interfaceShaderProgram           = -1;
std::int32_t interfaceOrthoUniform            = -1;
std::int32_t interfaceFontTextureUniform      = -1;
std::int32_t interfaceTextureOneUniform       = -1;
std::int32_t requestedWidth                   = -1;
std::int32_t requestedHeight                  = -1;

General GeneralConfig = {nullptr, 800, 600, 0.05f, 1e-6f, 60.0f, 64, 4};

std::uint32_t incEntityId()
{
  assert(std::atomic<std::uint32_t>::is_always_lock_free);
  return INTERNAL_ENTITY_ID.fetch_add(1, std::memory_order_relaxed);
}

std::uint32_t incUiId()
{
  assert(std::atomic<std::uint32_t>::is_always_lock_free);
  return INTERNAL_UI_ID.fetch_add(1, std::memory_order_relaxed);
}

}  // namespace Config
