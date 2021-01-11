/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CProjectileInfoSA.cpp
 *  PURPOSE:     Projectile type information
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CProjectileInfoSA.h"

extern CGameSA* pGame;

std::array<CProjectileInfoSA, 32>              CProjectilesSA::m_projectileInfo;
std::array<std::unique_ptr<CProjectileSA>, 32> CProjectilesSA::m_projectiles;

CEntity* CProjectileInfoSA::GetOwnerEntity() noexcept
{
    return const_cast<CEntity*>(static_cast<const CProjectileInfoSA*>(this)->GetOwnerEntity());
}

const CEntity* CProjectileInfoSA::GetOwnerEntity() const noexcept
{
    if (m_instance->ownerEntity == nullptr)
        return nullptr;

    return pGame->GetPools()->GetEntity(m_instance->ownerEntity);
}

void CProjectileInfoSA::SetOwnerEntity(CEntity* entity) noexcept
{
    m_instance->ownerEntity = (entity != nullptr) ? entity->GetInterface() : nullptr;
}

CEntity* CProjectileInfoSA::GetTargetEntity() noexcept
{
    return const_cast<CEntity*>(static_cast<const CProjectileInfoSA*>(this)->GetTargetEntity());
}

const CEntity* CProjectileInfoSA::GetTargetEntity() const noexcept
{
    if (m_instance->targetEntity == nullptr)
        return nullptr;

    return pGame->GetPools()->GetEntity(m_instance->targetEntity);
}

void CProjectileInfoSA::SetTargetEntity(CEntity* entity) noexcept
{
    m_instance->targetEntity = (entity != nullptr) ? entity->GetInterface() : nullptr;
}

CFxSystem* CProjectileInfoSA::GetParticle() noexcept
{
    return nullptr;
}

const CFxSystem* CProjectileInfoSA::GetParticle() const noexcept
{
    return nullptr;
}

CProjectilesSA::CProjectilesSA()
{
    auto gaProjectileInfo = reinterpret_cast<CProjectileInfoSAInterface*>(0xC891A8);

    for (size_t i = 0; i < 32; ++i)
    {
        CProjectileInfoSA& info = m_projectileInfo[i];
        info.m_index = i;
        info.m_instance = gaProjectileInfo;

        m_projectiles[i] = nullptr;

        ++gaProjectileInfo;
    }
}

bool CProjectilesSA::CreateProjectile(const CreateProjectileParams& params) noexcept
{
    using signature = bool(__cdecl *)(CEntitySAInterface*, eWeaponType, CVector, float, CVector*, CEntitySAInterface*);
    auto function = reinterpret_cast<signature>(0x737C80);

    auto                ownerGame = dynamic_cast<CEntitySA*>(params.ownerEntity);
    CEntitySAInterface* ownerInterface = ownerGame ? ownerGame->GetInterface() : nullptr;

    auto                targetGame = dynamic_cast<CEntitySA*>(params.targetEntity);
    CEntitySAInterface* targetInterface = targetGame ? targetGame->GetInterface() : nullptr;

    return function(ownerInterface, params.weapon, params.originPosition, params.force, nullptr, targetInterface);
}

bool CProjectilesSA::DestroyProjectile(CProjectile* projectile, bool blowUp) noexcept
{
    CProjectileSA* wrapper = dynamic_cast<CProjectileSA*>(projectile);

    if (!wrapper)
        return false;

    size_t index = wrapper->GetProjectileInfo()->GetIndex();
    auto   projectileInstance = reinterpret_cast<CProjectileSAInterface*>(wrapper->GetInterface());

    if (blowUp)
    {
        CProjectileInfoSAInterface* projectileInfoInstance = m_projectileInfo[index].m_instance;

        using signature = void(__cdecl*)(CProjectileInfoSAInterface*, CProjectileSAInterface*);
        auto function = reinterpret_cast<signature>(0x7388F0);
        function(projectileInfoInstance, projectileInstance);
    }
    else
    {
        using signature = bool(__cdecl *)(CProjectileSAInterface*);
        auto function = reinterpret_cast<signature>(0x739A40);
        function(projectileInstance);
    }

    m_projectileInfo[index].m_instance->isActive = false;
    m_projectiles[index].reset();
    return true;
}

CProjectile* CProjectilesSA::OnGameProjectileCreate(size_t index, intptr_t instance) noexcept
{
    auto projectile = new CProjectileSA(reinterpret_cast<CProjectileSAInterface*>(instance));
    projectile->m_info = &m_projectileInfo[index];
    projectile->m_index = index;

    m_projectiles[index].reset(projectile);
    return projectile;
}
