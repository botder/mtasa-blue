#include "StdInc.h"
#include "ResourceFilePath.h"
#include "ResourceManager.h"
#include "Resource.h"

namespace fs = std::filesystem;

namespace mtasa
{
    std::optional<ResourceFilePath> ParseResourceFilePath(std::string_view filePath, Resource* relativeTo)
    {
        // Empty, too long or directory paths are disallowed
        if (filePath.empty() || filePath.size() > 65535 || filePath.back() == '/' || filePath.back() == '\\')
            return std::nullopt;

        // Skip the '@'-character, which indicates private storage on clients and it isn't relevant on the server
        if (filePath[0] == '@')
            filePath = filePath.substr(1);

        std::string_view relativePath;

        // Check if the path follows the ":resourceName/path/to/example.txt" pattern
        if (filePath[0] == ':')
        {
            // Search for the first directory separator to separate resource name from relative resource path
            std::size_t resourceNameSeparator = filePath.find_first_of("/\\", 1);

            if (resourceNameSeparator == 1 || resourceNameSeparator == std::string_view::npos)
                return std::nullopt;

            std::string_view resourceName = filePath.substr(1, resourceNameSeparator - 2);
            relativeTo = g_pGame->GetResourceManager().GetResourceFromName(resourceName);

            relativePath = filePath.substr(resourceNameSeparator + 1);
        }
        else
        {
            relativePath = filePath;
        }

        if (relativeTo == nullptr || relativePath.empty())
            return std::nullopt;

#ifdef WIN32
        fs::path relativeFilePath = fs::path{relativePath}.lexically_normal();
#else
        // On non-Windows platforms, the backslash character is not a directory separator
        std::string relativePathString{resourceRelativePath};
        std::replace(relativePathString.begin(), relativePathString.end(), '\\', '/');
        fs::path relativeFilePath = fs::path{std::move(relativePathString)}.lexically_normal();
#endif

        if (relativeFilePath.empty() || relativeFilePath.is_absolute() || *relativeFilePath.begin() == "..")
            return std::nullopt;

        ResourceFilePath result;
        result.resource = relativeTo;
        result.relativePath = relativeFilePath;
        result.absolutePath = relativeTo->GetUnsafeAbsoluteFilePath(relativeFilePath);

        if (result.absolutePath.empty())
            return std::nullopt;

        return result;
    }
}            // namespace mtasa
