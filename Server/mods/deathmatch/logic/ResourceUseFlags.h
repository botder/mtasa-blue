/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Resource feature switches
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

namespace mtasa
{
    struct ResourceUseFlags
    {
        bool useDependencies = true;
        bool useServerConfigs = true;
        bool useServerMaps = true;
        bool useServerScripts = true;
        bool useHttpFiles = true;
        bool useClientConfigs = true;
        bool useClientScripts = true;
        bool useClientFiles = true;
    };
}
