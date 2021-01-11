/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CProjectileInfoSA.h
 *  PURPOSE:     Header file for projectile type information class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CProjectileSA.h"
#include <game/CProjectileInfo.h>

class CProjectileInfoSAInterface
{
public:
    eWeaponType           weaponType;
    CEntitySAInterface*   ownerEntity;
    CEntitySAInterface*   targetEntity;
    uint32_t              timeToDestroy;
    bool                  isActive;
    CVector               previousPosition;
    CFxSystemSAInterface* particle;
};
static_assert(sizeof(CProjectileInfoSAInterface) == 36, "invalid size of CProjectileInfoSAInterface");

class CProjectileInfoSA : public CProjectileInfo
{
public:
    CProjectileInfoSAInterface* m_instance = nullptr;
    size_t                      m_index = 0;

    size_t GetIndex() const noexcept override { return m_index; }

    eWeaponType GetWeaponType() const noexcept override { return m_instance->weaponType; }

    uint32_t GetTimeToDestroy() const noexcept override { return m_instance->timeToDestroy; }
    void     SetTimeToDestroy(uint32_t timeToDestroy) noexcept override { m_instance->timeToDestroy = timeToDestroy; }

    bool IsActive() const noexcept override { return m_instance->isActive; }
    void SetActive(bool isActive) noexcept override { m_instance->isActive = isActive; }

    CEntity*       GetOwnerEntity() noexcept override;
    const CEntity* GetOwnerEntity() const noexcept override;
    void           SetOwnerEntity(CEntity* entity) noexcept override;

    CEntity*       GetTargetEntity() noexcept override;
    const CEntity* GetTargetEntity() const noexcept override;
    void           SetTargetEntity(CEntity* entity) noexcept override;

    const CVector& GetPreviousPosition() const noexcept override { return m_instance->previousPosition; }

    CFxSystem*       GetParticle() noexcept override;
    const CFxSystem* GetParticle() const noexcept override;
};

class CProjectilesSA : public CProjectiles
{
public:
    CProjectilesSA();

    bool CreateProjectile(const CreateProjectileParams& params) noexcept override;
    bool DestroyProjectile(CProjectile* projectile, bool blowUp) noexcept override;

    CProjectileInfo*       GetProjectileInfo(size_t index) noexcept override { return &m_projectileInfo[index]; }
    const CProjectileInfo* GetProjectileInfo(size_t index) const noexcept override { return &m_projectileInfo[index]; }

    CProjectile*       GetProjectile(size_t index) noexcept override { return m_projectiles[index].get(); }
    const CProjectile* GetProjectile(size_t index) const noexcept override { return m_projectiles[index].get(); }

    CProjectile* OnGameProjectileCreate(size_t index, intptr_t instance) noexcept override;

private:
    static std::array<CProjectileInfoSA, 32>              m_projectileInfo;
    static std::array<std::unique_ptr<CProjectileSA>, 32> m_projectiles;
};
