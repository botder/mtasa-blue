/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CProjectileSA.h
 *  PURPOSE:     Header file for projectile entity class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CProjectile.h>
#include "CObjectSA.h"

class CProjectileInfoSA;

class CProjectileSAInterface : public CObjectSAInterface
{
};

class CProjectileSA : public virtual CProjectile, public virtual CObjectSA
{
public:
    CProjectileInfoSA* m_info = nullptr;
    size_t             m_index = 0;

    explicit CProjectileSA(CProjectileSAInterface* instance) : CObjectSA(instance) {}

    size_t GetIndex() const noexcept { return m_index; }

    CProjectileInfo*       GetProjectileInfo() noexcept override { return reinterpret_cast<CProjectileInfo*>(m_info); }
    const CProjectileInfo* GetProjectileInfo() const noexcept override { return reinterpret_cast<CProjectileInfo*>(m_info); }

    void Destroy(bool blow) noexcept override;
    bool CorrectPhysics() noexcept override;
};
