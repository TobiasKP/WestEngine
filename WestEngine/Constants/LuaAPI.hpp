
#pragma once

#include <string_view>

namespace LuaAPI
{
// C->Lua notifications, every event goes through this single entry point and is
// routed by the handler table in WestAPI.lua
constexpr std::string_view ON_EVENT = "OnEvent";

// C->Lua queries, these return a value and stay explicit
constexpr std::string_view GET_ACTION_POINTS = "GetActionPoints";
constexpr std::string_view GET_STATE         = "GetState";

// Table of EventIdentifiers the lua handlers key on, exported on startup
constexpr std::string_view C_EVENTS = "Events";

// Entity Creation
constexpr std::string_view C_CREATE_ENTITY = "createEntity";
constexpr std::string_view C_ADD_COMPONENT = "addComponent";
constexpr std::string_view C_BUILD_ENTITY  = "buildEntity";
constexpr std::string_view C_LOAD_WORLD    = "loadWorld";

// Entity Information
constexpr std::string_view C_EVENT       = "dispatchEvent";
constexpr std::string_view C_GETHEALTH   = "getHealth";
constexpr std::string_view C_GETPOSITION = "getPosition";

// AI Information
constexpr std::string_view C_GET_WORLD_INFO = "gatherWorldInformation";
constexpr std::string_view C_AI_MOVE        = "aiMoveCommand";
constexpr std::string_view C_AI_ATTACK      = "aiAttackCommand";
constexpr std::string_view C_AI_END         = "aiEndAction";

// General Calls
constexpr std::string_view C_GET_RESOLUTION    = "getScreenResolution";
constexpr std::string_view C_CREATE_INTERFACE  = "createInterface";
constexpr std::string_view C_UPDATE_INTERFACE  = "updateInterfaceValue";
constexpr std::string_view C_DESTROY_INTERFACE = "destroyInterface";
constexpr std::string_view C_GET_MOUSEPOS      = "getMousePosition";
}  // namespace LuaAPI
