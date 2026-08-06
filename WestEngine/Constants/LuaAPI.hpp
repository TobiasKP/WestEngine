
#pragma once

#include <string_view>

namespace LuaAPI
{
// C->Lua calls
constexpr std::string_view WORLD_POS_LCLICK  = "Worldpos_lclick";
constexpr std::string_view WORLD_POS_RCLICK  = "Worldpos_rclick";
constexpr std::string_view ENTITY_REGISTER   = "RegisterEntity";
constexpr std::string_view ENTITY_REMOVE     = "RemoveEntity";
constexpr std::string_view ENTITY_ATTACK     = "Entity_attack";
constexpr std::string_view ENTITY_RCLICK     = "Entity_rclick";
constexpr std::string_view STATE_CHANGE      = "EntityStateChange";
constexpr std::string_view GET_ACTION_POINTS = "GetActionPoints";
constexpr std::string_view UI_REFRESH        = "RefreshInterfaces";
constexpr std::string_view UI_DELETE         = "DestroyInterface";
constexpr std::string_view GET_STATE         = "GetState";
constexpr std::string_view INTERNAL          = "InterfaceInternalFunctionCall";
constexpr std::string_view ENTITY_QUEUE      = "EntityQueue";
constexpr std::string_view AI_THINK          = "AIThink";
constexpr std::string_view LEVEL_END         = "LevelEnd";

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
