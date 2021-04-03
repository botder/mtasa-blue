/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Resource start packet class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CResourceStartPacket.h"
#include "Resource.h"

bool CResourceStartPacket::Write(NetBitStreamInterface& BitStream) const
{
    // Write the resource name
    const std::string& resourceName = m_resource.GetName();

    if (resourceName.empty() || resourceName.size() > 0xFF)
        return false;

    auto resourceNameLength = static_cast<std::uint8_t>(resourceName.size());
    BitStream.Write(resourceNameLength);
    BitStream.Write(resourceName.c_str(), resourceNameLength);

    // Write the resource remote identifier
    BitStream.Write(m_resource.GetRemoteIdentifier());

    // Write the resource element id
    BitStream.Write(m_resource.GetElement()->GetID());

    // Write the resource dynamic element id
    BitStream.Write(m_resource.GetDynamicElementRoot()->GetID());

    // Count the amount of 'no client cache' scripts
    std::size_t clientScriptsCount = m_resource.GetClientNoCacheScriptsCount();

    if (clientScriptsCount > 0xFFFF)
        return false;

    auto uncachableScriptCount = static_cast<std::uint16_t>(clientScriptsCount);
    BitStream.Write(uncachableScriptCount);

    // Write the declared min client version for this resource
    if (BitStream.Version() >= 0x32)
    {
        BitStream.WriteString(m_resource.GetMinServerVersion());
        BitStream.WriteString(m_resource.GetMinClientVerison());
    }

    if (BitStream.Version() >= 0x45)
    {
        BitStream.WriteBit(m_resource.IsUsingOOP());
    }

    if (BitStream.Version() >= 0x62)
    {
        BitStream.Write(m_resource.GetDownloadPriorityGroup());
    }

    // Send the resource files info
    for (const ClientFile& file : m_files)
    {
        if (file.relativePath.size() > 0xFFFF)
            return false;

        // Write the type of chunk to read (F - File, E - Exported Function)
        BitStream.Write(static_cast<unsigned char>('F'));

        // Write the relative path of the file
        auto filePathLength = static_cast<std::uint16_t>(file.relativePath.size());
        BitStream.Write(filePathLength);

        if (filePathLength > 0)
            BitStream.Write(file.relativePath.data(), filePathLength);

        // Write the file type (mapped from eResourceType for network compatibility)
        std::uint8_t fileType = 0;

        switch (file.type)
        {
            case ClientFileType::FILE:
                fileType = 6; // = RESOURCE_FILE_TYPE_CLIENT_FILE
                break;
            case ClientFileType::CONFIG:
                fileType = 4; // = RESOURCE_FILE_TYPE_CLIENT_CONFIG
                break;
            case ClientFileType::SCRIPT:
                fileType = 2; // = RESOURCE_FILE_TYPE_CLIENT_SCRIPT
                break;
            default:
                return false;
        }

        BitStream.Write(fileType);

        // Write the checksum
        BitStream.Write(file.checksum.ulCRC);
        BitStream.Write(reinterpret_cast<const char*>(file.checksum.md5.data), sizeof(file.checksum.md5.data));

        // Write the approximate size of the file
        BitStream.Write(file.approximateSize);

        // Optionally write a bool whether the file is an optional download
        if (file.isOptional.has_value())
        {
            BitStream.WriteBit(*file.isOptional);
        }
    }

    // Loop through the exported functions
    for (const std::string& functionName : m_resource.GetExportedClientFunctions())
    {
        if (functionName.empty())
            continue;

        if (functionName.size() > 0xFF)
            return false;

        auto functionNameLength = static_cast<std::uint8_t>(functionName.size());

        // Write the type of chunk to read (F - File, E - Exported Function)
        BitStream.Write(static_cast<unsigned char>('E'));

        // Write the function name
        BitStream.Write(functionNameLength);
        BitStream.Write(functionName.c_str(), functionNameLength);
    }

    return true;
}
