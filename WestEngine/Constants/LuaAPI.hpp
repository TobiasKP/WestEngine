
#pragma once

#include <string_view>

namespace LuaAPI
{
constexpr std::string_view WORLD_POS_LCLICK = "Worldpos_lclick";
constexpr std::string_view WORLD_POS_RCLICK = "Worldpos_rclick";
constexpr std::string_view C_MOVE_PLAYER    = "MoveCurPlayer";
constexpr std::string_view C_ACTIONF_PLAYER = "ActionFinished";
constexpr std::string_view STATE_CHANGE     = "StateChange";
}  // namespace LuaAPI
