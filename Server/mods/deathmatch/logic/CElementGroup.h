/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CElementGroup.h
 *  PURPOSE:     Element group class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CElementGroup;

#pragma once

#include "CElement.h"

class CResource;

class CElementGroup
{
private:
    CFastList<CElement*> m_elements;
    CResource*           m_pResource;

public:
    CElementGroup(CResource* pResource);
    ~CElementGroup();
    void         Add(CElement* element);
    void         Remove(CElement* element);
    unsigned int GetCount(void);
    CResource*   GetResource() const { return m_pResource; }

    CFastList<CElement*>::const_iterator IterBegin(void) { return m_elements.begin(); }
    CFastList<CElement*>::const_iterator IterEnd(void) { return m_elements.end(); }
};
