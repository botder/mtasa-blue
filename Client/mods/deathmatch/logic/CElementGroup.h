/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CElementGroup.h
 *  PURPOSE:     Header for element group class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

class CClientEntity;
class CResource;

class CElementGroup
{
private:
    CFastList<CClientEntity*> m_elements;
    CResource*           m_pResource;

public:
    CElementGroup(CResource* const pResource);
    ~CElementGroup();

    void         Add(CClientEntity* element);
    void         Remove(CClientEntity* element);
    unsigned int GetCount(void);
    CResource*   GetResource() const { return m_pResource; }
};
