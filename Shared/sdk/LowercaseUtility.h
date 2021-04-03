#pragma once

#include "SharedUtil.Misc.h"
#include <string>
#include <algorithm>

namespace mtasa
{
    struct LowercaseHash
    {
        std::size_t operator()(const std::string& value) const
        {
            using SharedUtil::tolower;

            std::size_t result = 0;

            for (unsigned char c : value)
            {
                // See https://stackoverflow.com/a/107657
                result = result * 101 + tolower(c);
            }

            return result;
        }
    };

    struct LowercaseEqual
    {
        bool operator()(const std::string& lhs, const std::string& rhs) const
        {
            using SharedUtil::tolower;

            if (lhs.size() != rhs.size())
                return false;

            return std::equal(lhs.begin(), lhs.end(), rhs.begin(), [](unsigned char l, unsigned char r) { return tolower(l) == tolower(r); });
        }
    };
}
