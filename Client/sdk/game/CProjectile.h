/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CProjectile.h
 *  PURPOSE:     Projectile entity interface
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CObject.h"

class CProjectileInfo;

class CProjectile : public virtual CObject
{
public:
    virtual size_t GetIndex() const noexcept = 0;

    virtual CProjectileInfo*       GetProjectileInfo() noexcept = 0;
    virtual const CProjectileInfo* GetProjectileInfo() const noexcept = 0;

    virtual void Destroy(bool blow) noexcept = 0;
    virtual bool CorrectPhysics() noexcept = 0;
};
