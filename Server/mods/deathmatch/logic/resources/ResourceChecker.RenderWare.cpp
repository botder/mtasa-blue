/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     RenderWare resource file content checker/validator/upgrader
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <cstdint>
#include <filesystem>

namespace mtasa
{
    enum RwObjectId : std::uint32_t
    {
        RW_INVALID = 0x0,
        RW_STRUCT = 0x1,
        RW_EXTENSION = 0x3,
        RW_CAMERA = 0x5,
        RW_FRAMELIST = 0xE,
        RW_CLUMP = 0x10,
        RW_LIGHT = 0x12,
        RW_ATOMIC = 0x14,
        RW_TEXTURENATIVE = 0x15,
        RW_TEXDICTIONARY = 0x16,
        RW_GEOMETRYLIST = 0x1A,
    };

    struct RwStreamChunkHeader
    {
        std::uint32_t type = 0;
        std::uint32_t length = 0;
        std::uint32_t id = 0;
    };

    struct RwStreamTexDictionary
    {
        std::uint16_t textureCount = 0;
        std::uint16_t device = 0;
    };

    struct RwClumpChunkInfo
    {
        std::uint32_t atomicsCount = 0;
        std::uint32_t lightsCount = 0;
        std::uint32_t cameraCount = 0;
    };

    class RwFileStream
    {
    public:
        RwFileStream(const std::filesystem::path& filePath) : m_stream{File::Fopen(filePath.string().c_str(), "rb"), &fclose} {}

        operator bool() const noexcept { return m_stream != nullptr; }

        bool ReadChunk(RwStreamChunkHeader& header)
        {
            if (!m_stream)
                return false;

            return fread(&header, sizeof(header), 1, m_stream.get()) == 1;
        }
        
        bool FindChunk(std::uint32_t type, RwStreamChunkHeader& outHeader)
        {
            if (!m_stream)
                return false;

            RwStreamChunkHeader header;

            while (fread(&header, sizeof(header), 1, m_stream.get()) == 1)
            {
                if (header.type == type)
                {
                    outHeader = header;
                    return true;
                }

                if (fseek(m_stream.get(), header.length, SEEK_CUR) != 0)
                    return false;
            }

            return false;
        }

        bool SkipBytes(std::int32_t bytesCount)
        {
            if (!m_stream)
                return false;

            if (!bytesCount)
                return true;

            return fseek(m_stream.get(), bytesCount, SEEK_CUR) == 0;
        }

        bool ReadBytes(void* buffer, std::uint32_t bytesCount)
        {
            if (!m_stream)
                return false;

            return fread(buffer, bytesCount, 1, m_stream.get()) == 1;
        }

    private:
        std::unique_ptr<std::FILE, decltype(&fclose)> m_stream;
    };

    bool IsRenderWareFileHierarchyCorrupt(const std::filesystem::path& filePath)
    {
        RwFileStream stream{filePath};

        if (!stream)
            return false;

        RwStreamChunkHeader mainHeader;

        if (!stream.ReadChunk(mainHeader))
            return true;

        while (mainHeader.type != RW_TEXDICTIONARY && mainHeader.type != RW_CLUMP)
        {
            if (!stream.SkipBytes(mainHeader.length))
                return true;

            if (!stream.ReadChunk(mainHeader))
                return true;
        }

        if (mainHeader.type == RW_TEXDICTIONARY)
        {
            RwStreamChunkHeader   header;
            RwStreamTexDictionary texDictionary;

            if (!stream.FindChunk(RW_STRUCT, header) || !stream.ReadBytes(&texDictionary, sizeof(texDictionary)))
                return true;

            while (texDictionary.textureCount > 0)
            {
                if (!stream.FindChunk(RW_TEXTURENATIVE, header) || !stream.SkipBytes(header.length))
                    return true;

                --texDictionary.textureCount;
            }

            if (!stream.FindChunk(RW_EXTENSION, header) || !stream.SkipBytes(header.length))
                return true;

            return false;
        }
        else if (mainHeader.type == RW_CLUMP)
        {
            do
            {
                RwStreamChunkHeader header;
                RwClumpChunkInfo    clump;
                std::size_t         bytesRead = 0;

                if (!stream.FindChunk(RW_STRUCT, header) || !stream.ReadBytes(&clump, sizeof(clump)))
                    return true;

                bytesRead += sizeof(header) + header.length;

                if (!stream.FindChunk(RW_FRAMELIST, header) || !stream.SkipBytes(header.length))
                    return true;

                bytesRead += sizeof(header) + header.length;

                if (!stream.FindChunk(RW_GEOMETRYLIST, header) || !stream.SkipBytes(header.length))
                    return true;

                bytesRead += sizeof(header) + header.length;

                while (clump.atomicsCount > 0)
                {
                    if (!stream.FindChunk(RW_ATOMIC, header) || !stream.SkipBytes(header.length))
                        return true;

                    bytesRead += sizeof(header) + header.length;
                    --clump.atomicsCount;
                }

                while (clump.lightsCount > 0)
                {
                    if (!stream.FindChunk(RW_STRUCT, header) || !stream.SkipBytes(header.length))
                        return true;

                    bytesRead += sizeof(header) + header.length;

                    if (!stream.FindChunk(RW_LIGHT, header) || !stream.SkipBytes(header.length))
                        return true;

                    bytesRead += sizeof(header) + header.length;
                    --clump.lightsCount;
                }

                while (clump.cameraCount > 0)
                {
                    if (!stream.FindChunk(RW_STRUCT, header) || !stream.SkipBytes(header.length))
                        return true;

                    bytesRead += sizeof(header) + header.length;

                    if (!stream.FindChunk(RW_CAMERA, header) || !stream.SkipBytes(header.length))
                        return true;

                    --clump.cameraCount;
                    bytesRead += sizeof(header) + header.length;
                }

                if (!stream.FindChunk(RW_EXTENSION, header) || !stream.SkipBytes(header.length))
                    return true;

                bytesRead += sizeof(header) + header.length;

                if (bytesRead != mainHeader.length)
                    return true;
            } while (stream.FindChunk(RW_CLUMP, mainHeader));
        }

        return false;
    }
}
