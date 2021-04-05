/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Asynchronously execute commands on resources
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "ResourceCommands.h"
#include "Resource.h"

namespace mtasa
{
    void ResourceRefreshCommand::Execute() const
    {
        resource->GetManager().RefreshResource(resource->GetName());
    }

    void ResourceRestartCommand::Execute() const
    {
        if (resource->IsRunning() && !resource->Restart(useFlags))
        {
            CLogger::ErrorPrintf("Unable to restart resource %s\n", resource->GetName().c_str());
        }
    }

    void ResourceStopCommand::Execute() const
    {
        if (resource->IsRunning() && !resource->Stop())
        {
            CLogger::ErrorPrintf("Unable to stop resource %s\n", resource->GetName().c_str());
        }
    }
}            // namespace mtasa
