#pragma once

#include <cstdint>
#include <variant>


struct MousePayload
{
  double x;
  double y;
  double scroll;
};

struct KeyboardPayload
{
  std::int32_t key;
  std::int32_t action;
};

struct GamePayload
{};

struct EmptyPayload
{};

using EventPayload = std::variant<MousePayload, KeyboardPayload, GamePayload, EmptyPayload>;
