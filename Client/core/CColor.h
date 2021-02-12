/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Color class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CColor final
{
public:
    unsigned char R, G, B, A;

    constexpr CColor() noexcept : R(255), G(255), B(255), A(255) {}

    constexpr CColor(unsigned char _R, unsigned char _G, unsigned char _B, unsigned char _A = 255) noexcept
        : R(_R), G(_G), B(_B), A(_A) {}

    constexpr CColor(unsigned long value) noexcept
        : A(255), B(value & 0xFF), G((value >> 8) & 0xFF), R((value >> 16) & 0xFF) {}

    constexpr CColor& operator=(unsigned long value) noexcept
    {
        R = (value >> 16) & 0xFF;
        G = (value >> 8) & 0xFF;
        B = (value)&0xFF;
        return *this;
    }

    bool operator==(const CColor& other) const { return R == other.R && G == other.G && B == other.B && A == other.A; }
};
