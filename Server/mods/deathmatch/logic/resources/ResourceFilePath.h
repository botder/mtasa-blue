/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     A parser for absolute and relative resource file paths
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <string>
#include <string_view>
#include <optional>

namespace mtasa
{
    class Resource;

    struct ResourceFilePath
    {
        Resource*             resource = nullptr;
        std::filesystem::path relativePath;
        std::filesystem::path absolutePath;
    };

    // Parses both absolute and relative resource file paths
    // and provides the resulting resource, relative and absolute file paths.
    // This function early-exits for empty paths, too long paths (> 65535) or directory paths.
    // An absolute path follows the ':resourceName/path/to/file.txt' pattern.
    // A relative path is relative to the resource's directory root (read: relative to the meta.xml file).
    std::optional<ResourceFilePath> ParseResourceFilePath(std::string_view filePath, Resource* relativeTo);
}
