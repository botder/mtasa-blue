/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     In-game server browser server cache
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CServerList;
class CServerListItem;

class CServerCacheInterface
{
public:
    virtual ~CServerCacheInterface() {}
    virtual void SaveServerCache(bool bWaitUntilFinished) = 0;
    virtual void GetServerCachedInfo(CServerListItem* pItem) = 0;
    virtual void SetServerCachedInfo(const CServerListItem* pItem) = 0;
    virtual void GetServerListCachedInfo(CServerList* pList) = 0;
    virtual bool GenerateServerList(CServerList* pList) = 0;
};

CServerCacheInterface* GetServerCache();
