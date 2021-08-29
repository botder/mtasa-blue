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
        return static_cast<std::uint16_t>(bytes[0]) << 8 |
               static_cast<std::uint16_t>(bytes[1]) << 0;
    }

    static inline std::uint16_t StoreBigEndian16(std::uint16_t value)
    {
        std::array<std::uint8_t, 2> bytes{
            static_cast<std::uint8_t>(value >> 8),
            static_cast<std::uint8_t>(value >> 0),
        };
        std::copy(bytes.begin(), bytes.end(), reinterpret_cast<std::uint8_t*>(&value));
        return value;
    }

    static inline std::uint16_t LoadBigEndian32(std::uint32_t value)
    {
        std::array<std::uint8_t, 4> bytes{};
        std::copy_n(reinterpret_cast<std::uint8_t*>(&value), sizeof(value), bytes.data());
        return static_cast<std::uint16_t>(bytes[0]) << 24 |
               static_cast<std::uint16_t>(bytes[1]) << 16 |
               static_cast<std::uint16_t>(bytes[2]) << 8 |
               static_cast<std::uint16_t>(bytes[3]) << 0;
    }

    static inline std::uint16_t StoreBigEndian32(std::uint32_t value)
    {
        std::array<std::uint8_t, 4> bytes{
            static_cast<std::uint8_t>(value >> 24),
            static_cast<std::uint8_t>(value >> 16),
            static_cast<std::uint8_t>(value >> 8),
            static_cast<std::uint8_t>(value >> 0),
            };
        std::copy(bytes.begin(), bytes.end(), reinterpret_cast<std::uint8_t*>(&value));
        return value;
    }
}            // namespace mtasa
