/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     A server-side HTTP resource file
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "ResourceFile.h"

class CLuaMain;
class HttpResponse;

namespace mtasa
{
    class ResourceHttpFile final : public ResourceFile
    {
    public:
        using ResourceFile::ResourceFile;

        void SetIsRaw(bool isRaw) { m_isRaw = isRaw; }
        bool IsRaw() const { return m_isRaw; }

        void SetIsDefault(bool isDefault) { m_isDefault = isDefault; }
        bool IsDefault() const { return m_isDefault; }

        void SetIsACLRestricted(bool isACLRestricted) { m_isACLRestricted = isACLRestricted; }
        bool IsACLRestricted() const { return m_isACLRestricted; }

        void SetIsUsingOOP(bool useOOP) { m_usingOOP = useOOP; }
        bool IsUsingOOP() const { return m_usingOOP; }

    private:
        bool m_isRaw = false;
        bool m_isDefault = false;
        bool m_isACLRestricted = false;
        bool m_usingOOP = false;

        CLuaMain* m_httpLuaContext = nullptr;

        HttpResponse* m_httpReponse = nullptr;
        std::string   m_responseBuffer;
        std::string   m_responseMime;
        std::uint16_t m_responseCode = 200;
    };
}            // namespace mtasa
