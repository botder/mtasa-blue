/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Script file element class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "ScriptFile.h"
#include "Resource.h"

namespace fs = std::filesystem;

namespace mtasa
{
    bool ScriptFile::Load(OpenScriptFileMode fileMode)
    {
        if (m_stream)
            return false;

        std::FILE* stream = nullptr;

        switch (fileMode)
        {
            case OpenScriptFileMode::READ:
                stream = File::Fopen(m_absolutePath.string().c_str(), "rb");
                break;
            case OpenScriptFileMode::READ_WRITE:
                stream = File::Fopen(m_absolutePath.string().c_str(), "rb+");
                break;
            case OpenScriptFileMode::CREATE:
                std::error_code errorCode;
                fs::create_directories(m_absolutePath.parent_path(), errorCode);

                if (errorCode)
                    return false;

                stream = File::Fopen(m_absolutePath.string().c_str(), "wb+");
                break;
        }

        if (stream == nullptr)
            return false;

        m_stream = std::unique_ptr<std::FILE, decltype(&fclose)>{stream, &fclose};
        m_ownerResource->GetLuaContext()->OnOpenFile(m_absolutePath.string());
        return true;
    }

    void ScriptFile::Unload()
    {
        m_stream.reset();
    }

    bool ScriptFile::Flush() const
    {
        if (m_stream)
            return fflush(m_stream.get()) == 0;

        return false;
    }

    bool ScriptFile::TryRead(std::size_t byteCount, char* buffer, std::size_t bufferSize, std::size_t& bytesRead) const
    {
        if (byteCount > bufferSize)
            byteCount = bufferSize;

        if (m_stream)
        {
            bytesRead = fread(buffer, 1, byteCount, m_stream.get());
            return true;
        }

        return false;
    }

    bool ScriptFile::TryWrite(std::string_view buffer, std::size_t& bytesWritten) const
    {
        if (m_stream)
        {
            bytesWritten = fwrite(buffer.data(), 1, buffer.size(), m_stream.get());
            return true;
        }

        return false;
    }

    bool ScriptFile::TryGetPosition(std::size_t& position) const
    {
        if (m_stream)
        {
            long rawPosition = ftell(m_stream.get());

            if (rawPosition >= 0)
            {
                position = rawPosition;
                return true;
            }
        }

        return false;
    }

    bool ScriptFile::TrySetPosition(std::size_t position, std::size_t& afterSeekPosition) const
    {
        if (m_stream)
        {
            if (position > m_maxFileSize)
                position = m_maxFileSize;

            if (fseek(m_stream.get(), position, SEEK_SET) != 0)
                return false;

            long result = ftell(m_stream.get());

            if (result >= 0)
            {
                afterSeekPosition = result;
                return true;
            }
        }

        return false;
    }

    bool ScriptFile::TryGetSize(std::size_t& size) const
    {
        std::error_code errorCode;
        std::uintmax_t fileSize = fs::file_size(m_absolutePath, errorCode);

        if (errorCode)
            return false;

        size = fileSize;
        return true;
    }

    bool ScriptFile::IsEOF() const
    {
        if (m_stream)
            return feof(m_stream.get()) != 0;

        return false;
    }

    std::string ScriptFile::GetResourceRelativePath(Resource* relativeFrom) const
    {
        if (m_relativeToResource == relativeFrom)
        {
            return m_relativePath.generic_string();
        }
        else
        {
            return ":"s + m_relativeToResource->GetName() + "/"s + m_relativePath.generic_string();
        }
    }
}            // namespace mtasa
