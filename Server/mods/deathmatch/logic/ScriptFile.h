/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Script file element class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CElement.h"
#include <cstdio>
#include <string>
#include <filesystem>
#include <memory>

namespace mtasa
{
    class Resource;

    enum class OpenScriptFileMode
    {
        READ,
        READ_WRITE,
        CREATE,
    };

    class ScriptFile final : public CElement
    {
    public:
        ScriptFile(const std::filesystem::path& absolutePath, const std::filesystem::path& relativePath)
            : CElement(nullptr), m_absolutePath{absolutePath}, m_relativePath{relativePath}, m_stream{nullptr, &fclose}
        {
            m_absolutePath.make_preferred();
            m_iType = CElement::SCRIPTFILE;
            SetTypeName("file");
        }
        
        void SetOwnerResource(mtasa::Resource* resource) { m_ownerResource = resource; }

        mtasa::Resource*       GetOwnerResource() { return m_ownerResource; }
        const mtasa::Resource* GetOwnerResource() const { return m_ownerResource; }

        void SetRelativeToResource(mtasa::Resource* resource) { m_relativeToResource = resource; }

        mtasa::Resource*       GetRelativeToResource() { return m_relativeToResource; }
        const mtasa::Resource* GetRelativeToResource() const { return m_relativeToResource; }

        void        SetMaxFileSize(std::size_t maxFileSize) { m_maxFileSize = maxFileSize; }
        std::size_t GetMaxFileSize() const { return m_maxFileSize; }

        const SLuaDebugInfo& GetLuaDebugInfo() { return m_luaDebugInfo; }
        void                 SetLuaDebugInfo(const SLuaDebugInfo& luaDebugInfo) { m_luaDebugInfo = luaDebugInfo; }

        bool Load(OpenScriptFileMode fileMode);
        void Unload();
        bool Flush() const;
        bool TryRead(std::size_t byteCount, char* buffer, std::size_t bufferSize, std::size_t& bytesRead) const;
        bool TryWrite(std::string_view buffer, std::size_t& bytesWritten) const;
        bool TryGetPosition(std::size_t& position) const;
        bool TrySetPosition(std::size_t position, std::size_t& afterSeekPosition) const;
        bool TryGetSize(std::size_t& size) const;
        bool IsEOF() const;

        std::string GetResourceRelativePath(mtasa::Resource* relativeFrom) const;
        std::string GetRelativePath() const { return m_relativePath.generic_string(); }

    private:
        void Unlink() override {}
        bool ReadSpecialData(const int) override { return true; }

    private:
        mtasa::Resource*      m_ownerResource = nullptr;
        mtasa::Resource*      m_relativeToResource = nullptr;
        std::filesystem::path m_absolutePath;
        std::filesystem::path m_relativePath;
        std::size_t           m_maxFileSize = -1;

        std::unique_ptr<std::FILE, decltype(&fclose)> m_stream;

        SLuaDebugInfo m_luaDebugInfo;
    };
}
