/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Web server request
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "Request.h"
#include <sstream>

namespace mtasa::web
{
    constexpr bool iequals(const std::string_view& lhs, const std::string_view& rhs)
    {
        if (lhs.size() != rhs.size())
            return false;

        return std::equal(lhs.begin(), lhs.end(), rhs.begin(), [](char a, char b) { return tolower(a) == tolower(b); });
    }

    std::string URI::GetBaseAddress() const
    {
        std::stringstream result;

        if (!scheme.empty())
        {
            result << scheme << ":";

            if (!hostname.empty())
            {
                result << "//";

                if (!userinfo.empty())
                {
                    result << userinfo << "@";
                }

                result << hostname;

                if (!port.empty())
                {
                    result << ":" << port;
                }
            }
        }

        return result.str();
    }

    std::vector<Parameter> ParseQuery(std::string_view query)
    {
        std::vector<Parameter> result;
        std::size_t            offset = 0;

        while (offset != std::string_view::npos)
        {
            std::size_t      ampersand = query.find('&', offset);
            std::string_view parameter = query.substr(0, ampersand);

            if (!parameter.empty())
            {
                std::size_t equals = parameter.find('=');

                if (equals > 0)
                {
                    Parameter entry;
                    entry.name = parameter.substr(0, equals);
                    entry.value = parameter.substr(equals + 1);
                    result.emplace_back(std::move(entry));
                }
            }

            offset = (ampersand == std::string_view::npos) ? ampersand : (ampersand + 1);
        }

        return result;
    }

    bool Request::TryGetHeader(std::string_view name, Header& out) const
    {
        const std::vector<Header>* headers = GetHeaders();

        if (headers == nullptr)
            return false;

        for (const Header& header : *headers)
        {
            if (iequals(header.name, name))
            {
                out = header;
                return true;
            }
        }

        return false;
    }
}            // namespace mtasa::web
