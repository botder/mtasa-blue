/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Measuring and displaying a timing graph
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#define TIMING_GRAPH(name) \
    GetGraphStats()->AddTimingPoint( name );

//
// CGraphStatsInterface for measuring and displaying a timing graph
//
class CGraphStatsInterface
{
public:
    virtual ~CGraphStatsInterface() {}

    virtual void Draw() = 0;
    virtual void SetEnabled(bool bEnabled) = 0;
    virtual bool IsEnabled() = 0;
    virtual void AddTimingPoint(const char* szName) = 0;
};

CGraphStatsInterface* GetGraphStats();
