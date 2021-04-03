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

    std::optional<ResourceFilePath> ParseResourceFilePath(std::string_view filePath, Resource* relativeTo);
}
