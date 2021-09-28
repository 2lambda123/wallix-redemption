#pragma once

#include <cstdint>
#include <unordered_map>

#include "keyboard/keylayout.hpp"

namespace scancode
{
    const std::unordered_map<std::uint16_t, std::pair<std::uint8_t, std::uint8_t>> *get_scancodes_table_ptr(KeyLayout::KbdId LCID) noexcept;
    const std::unordered_map<std::uint16_t, std::pair<std::uint8_t, std::uint16_t>> *get_extended_scancodes_table_ptr(KeyLayout::KbdId LCID) noexcept;
}
