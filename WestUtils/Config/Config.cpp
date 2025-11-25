#include "../Include/Config.h"

namespace WESTUTILS Config
{

std::atomic<bool> PAUSE                              = false;
std::atomic<std::uint32_t> INTERNAL_ENTITY_ID       = 900000;
std::atomic<std::uint32_t> INTERNAL_UI_ID           = 1;
ThreadPool* THREADPOOL                               = new ThreadPool(4);
std::uint32_t interfaceShaderProgram                 = -1;
std::uint32_t interfaceOrthoUniform                  = -1;
std::uint32_t interfaceFontTextureUniform            = -1;
std::uint32_t interfaceTextureOneUniform             = -1;

General GeneralConfig = {800, 600, 0.05f, 1e-6f, 60.0f, 64};

}  // namespace WESTUTILS Config
