/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <d3d9.h>
#include <CVector.h>

// Vertex type used by the line3d batcher
struct SPDVertex
{
    static const uint FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
    float             x, y, z;
    DWORD             color;
};

#define WRITE_PD_VERTEX(buf,fX,fY,fZ,dwColor) \
        buf->x = fX; \
        buf->y = fY; \
        buf->z = fZ; \
        buf->color = dwColor; \
        buf++;

struct SLine3DItem
{
    CVector vecFrom;
    CVector vecTo;
    float   fWidth;
    ulong   ulColor;
};

//
// Batches 3D line drawing
//
class CLine3DBatcher
{
public:
    ZERO_ON_NEW
    CLine3DBatcher(bool bZTest);
    ~CLine3DBatcher();

    void OnDeviceCreate(IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY);
    void Flush();
    bool HasItems() { return !m_LineList.empty(); }
    void AddLine3D(const CVector& vecFrom, const CVector& vecTo, float fWidth, ulong ulColor);

protected:
    bool                     m_bZTest;
    IDirect3DDevice9*        m_pDevice;
    std::vector<SLine3DItem> m_LineList;
};
