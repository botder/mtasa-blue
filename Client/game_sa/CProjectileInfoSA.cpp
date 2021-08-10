/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CProjectileInfoSA.cpp
 *  PURPOSE:     Source file for projectile type information class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CProjectileSA.h"

extern CGameSA* pGame;

CProjectileInfoSAInterface* CProjectileInfoSA::ms_projectileInfos = reinterpret_cast<CProjectileInfoSAInterface*>(0xC891A8);
CProjectileSAInterface**    CProjectileInfoSA::ms_projectiles = reinterpret_cast<CProjectileSAInterface**>(0xC89110);

CProjectileInfoSA::CProjectileInfoSA(CProjectileInfoSA::CreateManagerTag)
    : m_projectileInfos(new CProjectileInfoSA*[INFO_COUNT]), m_projectiles(new CProjectileSA*[INFO_COUNT])
{
    for (std::size_t i = 0; i < INFO_COUNT; i++)
    {
        m_projectileInfos[i] = new CProjectileInfoSA(&ms_projectileInfos[i]);
        m_projectiles[i] = new CProjectileSA();
        m_projectiles[i]->SetProjectileInfo(m_projectileInfos[i]);
    }
}

CProjectileInfoSA::~CProjectileInfoSA()
{
    if (m_projectileInfos)
    {
        for (std::size_t i = 0; i < INFO_COUNT; i++)
        {
            delete m_projectiles[i];
            delete m_projectileInfos[i];
        }

        delete[] m_projectiles;
        delete[] m_projectileInfos;
    }
}

void CProjectileInfoSA::Initialise()
{
    using Signature = void(__cdecl*)();
    const auto function = reinterpret_cast<Signature>(0x737B40);
    function();
}

void CProjectileInfoSA::Shutdown()
{
    using Signature = void(__cdecl*)();
    const auto function = reinterpret_cast<Signature>(0x737BC0);
    function();
}

void CProjectileInfoSA::Update()
{
    using Signature = void(__cdecl*)();
    const auto function = reinterpret_cast<Signature>(0x738B20);
    function();
}

bool CProjectileInfoSA::AddProjectile(CEntity* creator, eWeaponType weapon, CVector origin, float force, CVector* targetPosition, CEntity* targetEntity)
{
    CEntitySAInterface* creatorInterface = creator ? creator->GetInterface() : nullptr;
    CEntitySAInterface* targetInterface = targetEntity ? targetEntity->GetInterface() : nullptr;

    if (creator)
        pGame->GetWorld()->IgnoreEntity(creator);

    using Signature = bool(__cdecl*)(CEntitySAInterface*, eWeaponType, CVector, float, CVector*, CEntitySAInterface*);
    const auto function = reinterpret_cast<Signature>(0x737C80);
    bool       result = function(creatorInterface, weapon, origin, force, targetPosition, targetInterface);

    pGame->GetWorld()->IgnoreEntity(nullptr);
    return result;
}

CProjectileInfo* CProjectileInfoSA::GetProjectileInfo(std::int32_t index)
{
    if (m_projectileInfos && index < INFO_COUNT)
        return m_projectileInfos[index];

    return nullptr;
}

CProjectile* CProjectileInfoSA::OnProjectileCreate(std::int32_t index, CProjectileSAInterface* projectile)
{
    if (!m_projectiles || !projectile || index < 0 || index >= INFO_COUNT)
        return nullptr;

    m_projectiles[index]->SetProjectileInterface(projectile);
    return m_projectiles[index];
}

bool CProjectileInfoSA::IsProjectileInRange(float minX, float maxX, float minY, float maxY, float minZ, float maxZ, bool destroyProjectiles)
{
    using Signature = bool(__cdecl*)(float, float, float, float, float, float, bool);
    const auto function = reinterpret_cast<Signature>(0x739860);
    return function(minX, maxX, minY, maxY, minZ, maxZ, destroyProjectiles);
}

void CProjectileInfoSA::RemoveAllProjectiles()
{
    using Signature = void(__cdecl*)();
    const auto function = reinterpret_cast<Signature>(0x7399B0);
    function();
}

void CProjectileInfoSA::RemoveDetonatorProjectiles()
{
    using Signature = void(__cdecl*)();
    const auto function = reinterpret_cast<Signature>(0x738860);
    function();
}

bool CProjectileInfoSA::RemoveIfThisIsAProjectile(CObject* projectile)
{
    CObjectSAInterface* projectileInterface = projectile ? projectile->GetObjectInterface() : nullptr;
    using Signature = bool(__cdecl*)(CObjectSAInterface*);
    const auto function = reinterpret_cast<Signature>(0x739A40);
    return function(projectileInterface);
}

void CProjectileInfoSA::RemoveNotAdd(CEntity* creator, eWeaponType weapon, CVector origin)
{
    CEntitySAInterface* creatorInterface = creator ? creator->GetInterface() : nullptr;
    using Signature = void(__cdecl*)(CEntitySAInterface*, eWeaponType, CVector);
    const auto function = reinterpret_cast<Signature>(0x737C00);
    function(creatorInterface, weapon, origin);
}

void CProjectileInfoSA::RemoveProjectile(CProjectileInfo* projectileInfo, CProjectile* projectile)
{
    CProjectileInfoSAInterface* projectileInfoInterface = projectileInfo ? projectileInfo->GetInterface() : nullptr;
    auto                        projectileInterface = projectile ? reinterpret_cast<CProjectileSAInterface*>(projectile->GetInterface()) : nullptr;
    using Signature = void(__cdecl*)(CProjectileInfoSAInterface*, CProjectileSAInterface*);
    const auto function = reinterpret_cast<Signature>(0x7388F0);
    function(projectileInfoInterface, projectileInterface);
}

void CProjectileInfoSA::RemoveFXSystem(bool destroyFxSystem)
{
    if (!m_interface)
        return;

    using Signature = void(__thiscall*)(CProjectileInfoSAInterface*, std::int8_t);
    const auto function = reinterpret_cast<Signature>(0x737B80);
    function(m_interface, destroyFxSystem);
}

void CProjectileInfoSA::SetCreator(CEntity* creator)
{
    if (!m_interface)
        return;

    m_interface->m_creator = creator ? creator->GetInterface() : nullptr;
}

CEntity* CProjectileInfoSA::GetCreator() const
{
    if (!m_interface || !m_interface->m_creator)
        return nullptr;

    CEntity* creator = nullptr;

    switch (m_interface->m_creator->nType)
    {
        case ENTITY_TYPE_PED:
        {
            SClientEntity<CPedSA>* entityPair = pGame->GetPools()->GetPed((DWORD*)m_interface->m_creator);
            creator = entityPair ? entityPair->pEntity : nullptr;
            break;
        }
        case ENTITY_TYPE_VEHICLE:
        {
            SClientEntity<CVehicleSA>* entityPair = pGame->GetPools()->GetVehicle((DWORD*)m_interface->m_creator);
            creator = entityPair ? entityPair->pEntity : nullptr;
            break;
        }
    }

    return creator;
}

void CProjectileInfoSA::SetTarget(CEntity* target)
{
    if (!m_interface)
        return;

    m_interface->m_target = target ? target->GetInterface() : nullptr;
}

CEntity* CProjectileInfoSA::GetTarget() const
{
    if (!m_interface || !m_interface->m_target)
        return nullptr;

    CEntity* target = nullptr;

    switch (m_interface->m_target->nType)
    {
        case ENTITY_TYPE_PED:
        {
            SClientEntity<CPedSA>* entityPair = pGame->GetPools()->GetPed((DWORD*)m_interface->m_target);
            target = entityPair ? entityPair->pEntity : nullptr;
            break;
        }
        case ENTITY_TYPE_VEHICLE:
        {
            SClientEntity<CVehicleSA>* entityPair = pGame->GetPools()->GetVehicle((DWORD*)m_interface->m_target);
            target = entityPair ? entityPair->pEntity : nullptr;
            break;
        }
    }

    return target;
}

void CProjectileInfoSA::SetDestroyTime(std::int32_t destroyTime)
{
    if (m_interface)
        m_interface->m_destroyTime = destroyTime + pGame->GetSystemTime();
}

std::int32_t CProjectileInfoSA::GetDestroyTime() const
{
    if (!m_interface)
        return 0;

    return m_interface->m_destroyTime - pGame->GetSystemTime();
}

CFxSystem* CProjectileInfoSA::GetFxSystem() const
{
    if (!m_interface || !m_interface->m_fxSystem)
        return nullptr;

    return pGame->GetFxManagerSA()->GetFxSystem(m_interface->m_fxSystem);
}
