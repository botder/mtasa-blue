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

class CProjectileInfo;

class CProjectileSAInterface : public CObjectSAInterface
{
};

class CProjectileSA : public virtual CProjectile, public virtual CObjectSA
{
private:
    CProjectileInfo* m_projectileInfo = nullptr;
    bool             m_isDestroyed = true;

public:
    CProjectileSA();
    ~CProjectileSA();

    void Destroy(bool bBlow = true);
    void SetProjectileInfo(CProjectileInfo* pProjectileInfo) { m_projectileInfo = pProjectileInfo; }
    bool CorrectPhysics();

    void SetProjectileInterface(CProjectileSAInterface* projectile);
};
