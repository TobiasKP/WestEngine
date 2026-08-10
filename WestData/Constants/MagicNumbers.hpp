#pragma once

#include <string_view>

namespace MagicNumbers
{


constexpr std::string_view MAGIC_NUMBER = "WEST";
constexpr std::string_view MESH         = "M";
constexpr std::string_view VERTICE      = "V";
constexpr std::string_view INDICE       = "I";
constexpr std::string_view TEXTURE      = "T";
constexpr std::string_view IMG_DATA     = "ID";
constexpr std::string_view IMG_TYPE     = "IT";
constexpr std::string_view MATERIAL     = "MA";
constexpr std::string_view KD           = "Kd";
constexpr std::string_view KE           = "Ke";
constexpr std::string_view KS           = "Ks";
constexpr std::string_view DELIMITER    = "|";

// constexpr std::string_view MESH_OFFSET         = "MO";
};  // namespace MagicNumbers
