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

#include <cstdint>

struct CProjectileInfoSAInterface;
class CProjectileSAInterface;

class CProjectileInfo
{
protected:
    ~CProjectileInfo() = default;

public:
    // static method
    virtual void Initialise() = 0;

    // static method
    virtual void Shutdown() = 0;

    // static method
    virtual void Update() = 0;

    // static method
    virtual bool AddProjectile(CEntity* creator, eWeaponType weapon, CVector origin, float force, CVector* targetPosition, CEntity* targetEntity) = 0;

    // static method
    virtual CProjectileInfo* GetProjectileInfo(std::int32_t index) = 0;

    // static method
    virtual bool IsProjectileInRange(float minX, float maxX, float minY, float maxY, float minZ, float maxZ, bool destroyProjectiles) = 0;

    // static method
    virtual void RemoveAllProjectiles() = 0;

    // static method
    virtual void RemoveDetonatorProjectiles() = 0;

    // static method
    virtual bool RemoveIfThisIsAProjectile(CObject* projectile) = 0;

    // static method
    virtual void RemoveNotAdd(CEntity* creator, eWeaponType weapon, CVector origin) = 0;

    // static method
    virtual void RemoveProjectile(CProjectileInfo* projectileInfo, CProjectile* projectile) = 0;

    // static method (for multiplayer_sa hook)
    virtual CProjectile* OnProjectileCreate(std::int32_t index, CProjectileSAInterface* projectile) = 0;

public:
    virtual CProjectileInfoSAInterface* GetInterface() const = 0;

    virtual void RemoveFXSystem(bool destroyFxSystem) = 0;

    virtual bool IsActive() const = 0;

    virtual eWeaponType GetWeaponType() const = 0;

    virtual void SetCreator(CEntity* creator) = 0;

    virtual CEntity* GetCreator() const = 0;

    virtual void SetTarget(CEntity* target) = 0;

    virtual CEntity* GetTarget() const = 0;

    virtual void SetDestroyTime(std::int32_t destroyTime) = 0;

    virtual std::int32_t GetDestroyTime() const = 0;

    virtual CFxSystem* GetFxSystem() const = 0;
};
