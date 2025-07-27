#include <cstdint>

namespace BitMasks {
namespace General {
constexpr std::uint8_t MENU{0b0000'0001};
} // namespace General
namespace Control {
constexpr std::uint8_t CAMERA_MOVING{0b0000'0001};
constexpr std::uint8_t PLAYER_MOVING{0b0000'0010};
} // namespace Control
namespace Components {
constexpr std::uint16_t MODEL{0b0000'0000'0000'0001};
constexpr std::uint16_t SHADER{0b0000'0000'0000'0010};
constexpr std::uint16_t POSITION{0b0000'0000'0000'0100};
constexpr std::uint16_t TEXT{0b0000'0000'0000'1000};
} // namespace Components
}; // namespace BitMasks
