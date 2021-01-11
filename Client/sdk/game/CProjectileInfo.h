/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CProjectileInfo.h
 *  PURPOSE:     Projectile entity information interface
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CProjectileInfo
{
public:
    virtual size_t GetIndex() const noexcept = 0;

    virtual eWeaponType GetWeaponType() const noexcept = 0;

    virtual uint32_t GetTimeToDestroy() const noexcept = 0;
    virtual void     SetTimeToDestroy(uint32_t timeToDestroy) noexcept = 0;

    virtual bool IsActive() const noexcept = 0;
    virtual void SetActive(bool isActive) noexcept = 0;

    virtual CEntity*       GetOwnerEntity() noexcept = 0;
    virtual const CEntity* GetOwnerEntity() const noexcept = 0;
    virtual void           SetOwnerEntity(CEntity* entity) noexcept = 0;

    virtual CEntity*       GetTargetEntity() noexcept = 0;
    virtual const CEntity* GetTargetEntity() const noexcept = 0;
    virtual void           SetTargetEntity(CEntity* entity) noexcept = 0;

    virtual const CVector& GetPreviousPosition() const noexcept = 0;

    virtual CFxSystem*       GetParticle() noexcept = 0;
    virtual const CFxSystem* GetParticle() const noexcept = 0;
};

struct CreateProjectileParams
{
    CEntity*    ownerEntity;
    CEntity*    targetEntity;
    CVector     originPosition;
    eWeaponType weapon;
    float       force;
};

class CProjectiles
{
public:
    virtual bool CreateProjectile(const CreateProjectileParams& params) noexcept = 0;
    virtual bool DestroyProjectile(CProjectile* projectile, bool blowUp) noexcept = 0;

    virtual CProjectileInfo*       GetProjectileInfo(size_t index) noexcept = 0;
    virtual const CProjectileInfo* GetProjectileInfo(size_t index) const noexcept = 0;

    virtual CProjectile*       GetProjectile(size_t index) noexcept = 0;
    virtual const CProjectile* GetProjectile(size_t index) const noexcept = 0;

    virtual CProjectile* OnGameProjectileCreate(size_t index, intptr_t instance) noexcept = 0;
};
