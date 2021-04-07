/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Provide a resource from an archive file
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Resource.h"
#include <string_view>
#include <unordered_set>

namespace mtasa
{
    class ArchiveResource final : public Resource
    {
    public:
        ArchiveResource(ResourceManager& resourceManager)
            : Resource(resourceManager)
        {
        }

        void SetSourceArchive(const std::filesystem::path& sourceArchive) { m_sourceArchive = sourceArchive; }

        bool IsArchived() const override { return true; }

        bool Load() override { return PreProcessArchive() && Resource::Load(); }

        bool Start(ResourceUseFlags useFlags = {}) override { return DecompressArchive() && Resource::Start(useFlags); }

        bool Exists() const override;

        bool HasChanged() const override;

    protected:
        bool SourceFileExists(const std::filesystem::path& relativePath) const override;

    private:
        bool PreProcessArchive();
        bool DecompressArchive();

        std::filesystem::path m_sourceArchive;

        struct Hasher
        {
            std::size_t operator()(const std::filesystem::path& path) const noexcept { return std::filesystem::hash_value(path); }
        };

        std::unordered_set<std::filesystem::path, Hasher> m_archiveSourceFiles;
    };
}            // namespace mtasa
