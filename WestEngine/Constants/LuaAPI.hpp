
#pragma once

#include <string_view>

namespace LuaAPI
{
// C->Lua calls
constexpr std::string_view WORLD_POS_LCLICK = "Worldpos_lclick";
constexpr std::string_view WORLD_POS_RCLICK = "Worldpos_rclick";
constexpr std::string_view STATE_CHANGE     = "StateChange";
constexpr std::string_view UI_REFRESH       = "RefreshInterfaces";

// Player Actions
constexpr std::string_view C_MOVE_PLAYER    = "MoveCurPlayer";
constexpr std::string_view C_ACTIONF_PLAYER = "ActionFinished";

// Entity Creation
constexpr std::string_view C_CREATE_ENTITY = "createEntity";
constexpr std::string_view C_ADD_COMPONENT = "addComponent";
constexpr std::string_view C_BUILD_ENTITY  = "buildEntity";
constexpr std::string_view C_LOAD_WORLD    = "loadWorld";

// General Calls
constexpr std::string_view C_GET_RESOLUTION    = "getScreenResolution";
constexpr std::string_view C_CREATE_INTERFACE  = "createInterface";
constexpr std::string_view C_UPDATE_INTERFACE  = "updateInterfaceValue";
constexpr std::string_view C_DESTROY_INTERFACE = "destroyInterface";
}  // namespace LuaAPI
