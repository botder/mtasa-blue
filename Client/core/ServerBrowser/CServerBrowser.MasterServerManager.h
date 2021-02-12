/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     In-game server browser server manager
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CServerListItemList;

class CMasterServerManagerInterface
{
public:
    virtual ~CMasterServerManagerInterface() {}

    // CMasterServerManagerInterface
    virtual void Refresh() = 0;
    virtual bool HasData() = 0;
    virtual bool ParseList(CServerListItemList& itemList) = 0;
};

CMasterServerManagerInterface* NewMasterServerManager();
