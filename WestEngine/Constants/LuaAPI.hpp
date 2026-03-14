
#pragma once

#include <string_view>

namespace LuaAPI
{
// C->Lua calls
constexpr std::string_view WORLD_POS_LCLICK = "Worldpos_lclick";
constexpr std::string_view WORLD_POS_RCLICK = "Worldpos_rclick";
constexpr std::string_view ENTITY_LCLICK    = "Entity_lclick";
constexpr std::string_view ENTITY_RCLICK    = "Entity_rclick";
constexpr std::string_view STATE_CHANGE     = "StateChange";
constexpr std::string_view UI_REFRESH       = "RefreshInterfaces";
constexpr std::string_view UI_DELETE        = "DestroyInterface";
constexpr std::string_view GET_STATE        = "GetState";

// Entity Creation
constexpr std::string_view C_CREATE_ENTITY = "createEntity";
constexpr std::string_view C_ADD_COMPONENT = "addComponent";
constexpr std::string_view C_BUILD_ENTITY  = "buildEntity";
constexpr std::string_view C_LOAD_WORLD    = "loadWorld";

// Entity Information
constexpr std::string_view C_GETHEALTH = "getHealth";

// General Calls
constexpr std::string_view C_GET_RESOLUTION    = "getScreenResolution";
constexpr std::string_view C_CREATE_INTERFACE  = "createInterface";
constexpr std::string_view C_UPDATE_INTERFACE  = "updateInterfaceValue";
constexpr std::string_view C_DESTROY_INTERFACE = "destroyInterface";
constexpr std::string_view C_GET_MOUSEPOS      = "getMousePosition";
}  // namespace LuaAPI
