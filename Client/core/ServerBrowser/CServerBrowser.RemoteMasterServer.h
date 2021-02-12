/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     In-game server browser master server
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CServerListItemList;

// Communicate with a remote master server and parse the result.
class CRemoteMasterServerInterface : public CRefCountable
{
protected:
    virtual ~CRemoteMasterServerInterface() {}

public:
    // CRemoteMasterServerInterface
    virtual void Refresh() = 0;
    virtual bool HasData() = 0;
    virtual bool ParseList(CServerListItemList& itemList) = 0;
};

CRemoteMasterServerInterface* NewRemoteMasterServer(const SString& strURL);
