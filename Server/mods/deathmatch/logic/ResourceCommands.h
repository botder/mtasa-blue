/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Asynchronously execute commands on resources
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "ResourceUseFlags.h"

namespace mtasa
{
    class Resource;

    enum class ResourceCommandType
    {
        NONE,
        REFRESH,
        RESTART,
        STOP,
    };

    class ResourceCommand
    {
    public:
        Resource*           resource = nullptr;
        ResourceCommandType type = ResourceCommandType::NONE;

        ResourceCommand(Resource* resource_) : resource{resource_} {}

        virtual ~ResourceCommand() = default;
        virtual void Execute() const = 0;
    };

    class ResourceRefreshCommand : public ResourceCommand
    {
    public:
        ResourceRefreshCommand(Resource* resource) : ResourceCommand(resource) { type = ResourceCommandType::REFRESH; }

        void Execute() const override;
    };

    class ResourceRestartCommand : public ResourceCommand
    {
    public:
        ResourceUseFlags useFlags;

        ResourceRestartCommand(Resource* resource) : ResourceCommand(resource) { type = ResourceCommandType::RESTART; }

        void Execute() const override;
    };

    class ResourceStopCommand : public ResourceCommand
    {
    public:
        ResourceStopCommand(Resource* resource) : ResourceCommand(resource) { type = ResourceCommandType::STOP; }

        void Execute() const override;
    };
}
