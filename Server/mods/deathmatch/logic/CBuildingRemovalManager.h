/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Header file for building removal
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 * 
 *****************************************************************************/

#pragma once

#include <map>

class CBuildingRemoval;

class CBuildingRemovalManager
{
public:
    CBuildingRemovalManager();
    ~CBuildingRemovalManager();
    void CreateBuildingRemoval(unsigned short usModel, float fRadius, const CVector& vecPos, char cInterior);
    void ClearBuildingRemovals();
    void RestoreWorldModel(unsigned short usModel, float fRadius, const CVector& vecPos, char cInterior);
    std::multimap<unsigned short, CBuildingRemoval*>::const_iterator IterBegin() { return m_BuildingRemovals.begin(); };
    std::multimap<unsigned short, CBuildingRemoval*>::const_iterator IterEnd() { return m_BuildingRemovals.end(); };

private:
    std::multimap<unsigned short, CBuildingRemoval*> m_BuildingRemovals;
};
