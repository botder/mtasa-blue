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

#include <game/CProjectileInfo.h>

class CProjectileSA;
class CProjectileSAInterface;

struct CProjectileInfoSAInterface
{
    eWeaponType           m_weaponType;
    CEntitySAInterface*   m_creator;
    CEntitySAInterface*   m_target;
    std::int32_t          m_destroyTime;
    bool                  m_isActive;
    CVector               m_lastPosition;
    CFxSystemSAInterface* m_fxSystem;
};
static_assert(sizeof(CProjectileInfoSAInterface) == 0x24, "Invalid size for CProjectileInfoSAInterface");

class CProjectileInfoSA final : public CProjectileInfo
{
    CProjectileInfoSAInterface* m_interface = nullptr;
    CProjectileInfoSA**         m_projectileInfos = nullptr;
    CProjectileSA**             m_projectiles = nullptr;

public:
    static constexpr std::size_t INFO_COUNT = 32;

    static CProjectileInfoSAInterface* ms_projectileInfos;
    static CProjectileSAInterface**    ms_projectiles;

    struct CreateManagerTag {};

    CProjectileInfoSA(CreateManagerTag);

    CProjectileInfoSA(CProjectileInfoSAInterface* projectileInfo) : m_interface(projectileInfo) {}

    ~CProjectileInfoSA();

public:
    void             Initialise() override;
    void             Shutdown() override;
    void             Update() override;
    bool             AddProjectile(CEntity* creator, eWeaponType weapon, CVector origin, float force, CVector* targetPosition, CEntity* targetEntity) override;
    CProjectileInfo* GetProjectileInfo(std::int32_t index) override;
    CProjectile*     OnProjectileCreate(std::int32_t index, CProjectileSAInterface* projectile) override;
    bool             IsProjectileInRange(float minX, float maxX, float minY, float maxY, float minZ, float maxZ, bool destroyProjectiles) override;
    void             RemoveAllProjectiles() override;
    void             RemoveDetonatorProjectiles() override;
    bool             RemoveIfThisIsAProjectile(CObject* projectile) override;
    void             RemoveNotAdd(CEntity* creator, eWeaponType weapon, CVector origin) override;
    void             RemoveProjectile(CProjectileInfo* projectileInfo, CProjectile* projectile) override;

public:
    CProjectileInfoSAInterface* GetInterface() const override { return m_interface; }
    void                        RemoveFXSystem(bool destroyFxSystem) override;
    bool                        IsActive() const override { return m_interface ? m_interface->m_isActive : false; }
    eWeaponType                 GetWeaponType() const override { return m_interface ? m_interface->m_weaponType : WEAPONTYPE_UNARMED; }
    void                        SetCreator(CEntity* creator) override;
    CEntity*                    GetCreator() const override;
    void                        SetTarget(CEntity* target) override;
    CEntity*                    GetTarget() const override;
    void                        SetDestroyTime(std::int32_t destroyTime) override;
    std::int32_t                GetDestroyTime() const override;
    CFxSystem*                  GetFxSystem() const override;
};
