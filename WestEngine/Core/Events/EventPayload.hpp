#pragma once

#include <cstdint>
#include <string>
#include <variant>

struct MousePayload
{
  double x      = 0.0;
  double y      = 0.0;
  double scroll = 0.0;
};

struct KeyboardPayload
{
  std::int32_t key    = 0;
  std::int32_t action = 0;
};

struct GamePayload
{
  std::int32_t turn = 0;
};

struct AttackPayload
{
  std::uint32_t attacker = 0;
  std::uint32_t target   = 0;
};

struct InterfacePayload
{
  std::uint32_t event    = 0;
  std::uint32_t entityId = 0;
  std::string newValue;
};

struct ActionFinishedPayload
{
  std::uint32_t entityId = 0;
};

struct EmptyPayload
{};

using EventPayload = std::variant<MousePayload,
                                  KeyboardPayload,
                                  GamePayload,
                                  AttackPayload,
                                  EmptyPayload,
                                  InterfacePayload,
                                  ActionFinishedPayload>;
