#pragma once

#include <cstddef>
#include <string_view>

#define WEST_EVENT_LIST(X)                                                                                             \
  X(GAME_EVENT)                                                                                                        \
  X(ATTACK_EVENT)                                                                                                      \
  X(MOUSE_MOVE)                                                                                                        \
  X(MOUSE_LCLICK)                                                                                                      \
  X(MOUSE_RCLICK)                                                                                                      \
  X(MOUSE_WHEEL)                                                                                                       \
  X(KEY)                                                                                                               \
  X(INTERFACE_UPDATE)                                                                                                  \
  X(ACTION_FINISHED)                                                                                                   \
  X(ENTITY_CREATED)                                                                                                    \
  X(ENTITY_DESTROYED)                                                                                                  \
  X(ENTITY_STATE_CHANGE)                                                                                               \
  X(ENTITY_RCLICK)                                                                                                     \
  X(ENTITY_ATTACK)                                                                                                     \
  X(ENTITY_QUEUE)                                                                                                      \
  X(TILE_LCLICK)                                                                                                       \
  X(TILE_RCLICK)                                                                                                       \
  X(UI_REFRESH)                                                                                                        \
  X(UI_INTERNAL_CALL)                                                                                                  \
  X(AI_THINK)                                                                                                          \
  X(LEVEL_END)

enum class EventIdentifiers {
#define WEST_EVENT_ENUM_ENTRY(name) name,
  WEST_EVENT_LIST(WEST_EVENT_ENUM_ENTRY)
#undef WEST_EVENT_ENUM_ENTRY
};

#define WEST_EVENT_COUNT_ENTRY(name) +1
constexpr std::size_t EVENT_COUNT = 0 WEST_EVENT_LIST(WEST_EVENT_COUNT_ENTRY);
#undef WEST_EVENT_COUNT_ENTRY

constexpr std::string_view eventName(const EventIdentifiers event)
{
  switch (event)
  {
#define WEST_EVENT_NAME_ENTRY(name)                                                                                    \
  case EventIdentifiers::name:                                                                                         \
    return #name;
    WEST_EVENT_LIST(WEST_EVENT_NAME_ENTRY)
#undef WEST_EVENT_NAME_ENTRY
  }
  return "UNKNOWN_EVENT";
}
