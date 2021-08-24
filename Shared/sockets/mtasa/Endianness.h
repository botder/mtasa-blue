/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/sockets/mtasa/Endianness.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <type_traits>
#include <algorithm>
#include <cstdint>
#include <array>

namespace mtasa
{
    static inline std::uint16_t LoadBigEndian16(std::uint16_t value)
    {
        std::array<std::uint8_t, 2> bytes{};
        std::copy_n(reinterpret_cast<std::uint8_t*>(&value), sizeof(value), bytes.data());
        return static_cast<std::uint16_t>(bytes[0]) << 8 | static_cast<std::uint16_t>(bytes[1]);
    }

    static inline std::uint16_t StoreBigEndian16(std::uint16_t value)
    {
        std::array<std::uint8_t, 2> bytes{
            static_cast<std::uint8_t>(value >> 8),
            static_cast<std::uint8_t>(value),
        };
        std::copy(bytes.begin(), bytes.end(), reinterpret_cast<std::uint8_t*>(&value));
        return value;
    }
}            // namespace mtasa
