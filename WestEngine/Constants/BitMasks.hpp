#include <cstdint>

namespace BitMasks
{
namespace General
{
constexpr std::uint8_t MENU{0b0000'0001};
constexpr std::uint8_t INFO{0b0000'0010};
}  // namespace General
namespace Control
{
constexpr std::uint8_t CAMERA_MOVING{0b0000'0001};
constexpr std::uint8_t PLAYER_MOVING{0b0000'0010};
constexpr std::uint8_t UI_HOVERED{0b0000'0100};
constexpr std::uint8_t UI_UNHOVERED{0b0000'1000};
constexpr std::uint8_t UI_CLICKED{0b0001'0000};
constexpr std::uint8_t CAMERA_ZOOM{0b0010'0000};
}  // namespace Control

};  // namespace BitMasks
