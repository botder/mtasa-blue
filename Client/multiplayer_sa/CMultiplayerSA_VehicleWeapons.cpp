/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_VehicleWeapons.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#include "StdInc.h"

VehicleWeaponHitHandler* pVehicleWeaponHitHandler = nullptr;

void CMultiplayerSA::SetVehicleWeaponHitHandler(VehicleWeaponHitHandler* pHandler)
{
    pVehicleWeaponHitHandler = pHandler;
}

static void TriggerVehicleWeaponHitEvent(EVehicleWeaponType weaponType, CVehicleSAInterface* pGameVehicle, CEntitySAInterface* pHitGameEntity,
                                         CVector& vecPosition, int iModel, int iSurfaceType)
{
    if (!pVehicleWeaponHitHandler)
        return;

    SVehicleWeaponHitEvent event = {
        weaponType, pGameVehicle, pHitGameEntity, vecPosition, iModel, iSurfaceType,
    };

    pVehicleWeaponHitHandler(event);
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CWaterCannon::Render()
//
// Calculates the destination of the water stream, renders the splash fx there and
// informs the water cannon audio entity.
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x729324 | 0F 84 1C 02 00 00 | jz    0x729546
// >>> 0x72932A | 68 CD CC 4C 3E    | push  3E4CCCCDh
//     0x72932F | 68 00 00 80 3F    | push  3F800000h
#define HOOKPOS_CWaterCannon__Render         0x72932A
#define HOOKSIZE_CWaterCannon__Render        5
static DWORD CONTINUE_CWaterCannon__Render = 0x72932F;

static void HandleWaterCannonHit(CVehicleSAInterface* pGameVehicle, CColPointSAInterface* pColPoint, CEntitySAInterface** ppHitGameEntity)
{
    if (!pVehicleWeaponHitHandler)
        return;

    CEntitySAInterface* const pHitGameEntity = ppHitGameEntity ? *ppHitGameEntity : nullptr;
    const int                 iModel = pHitGameEntity ? pHitGameEntity->m_nModelIndex : -1;

    TriggerVehicleWeaponHitEvent(EVehicleWeaponType::WATER_CANNON, pGameVehicle, pHitGameEntity, pColPoint->Position, iModel, pColPoint->ucSurfaceTypeB);
}

static void _declspec(naked) HOOK_CWaterCannon__Render()
{
    _asm
    {
        pushad
        mov     eax, [ebx]                      // CVehicleSAInterface* CWaterCannon::m_pVehicle
        lea     ebx, [esp + 100h - 54h]         // CColPointSAInterface*
        lea     ecx, [esp + 100h - 58h]         // CEntitySAInterface**
        push    ecx                             // ppHitGameEntity
        push    ebx                             // pColPoint
        push    eax                             // pGameVehicle
        call    HandleWaterCannonHit
        add     esp, 12
        popad
        push    3E4CCCCDh
        jmp     CONTINUE_CWaterCannon__Render
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CAutomobile::TankControl
//
// Handles the vehicle firing with the RC Tiger and Rhino vehicle models. The hook below
// is only for Rhino.
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x6AF0F1 | D9 5C 24 6C                   | fstp [esp+9Ch+var_38.z]
// >>> 0x6AF0F5 | E8 A6 1E DF FF                | call Fx_c::TriggerTankFire(CVector&, CVector&)
//     0x6AF0FA | 5F                            | pop edi
#define HOOKPOS_CAutomobile__TankControl               0x6AF0F5
#define HOOKSIZE_CAutomobile__TankControl              5
static const DWORD CONTINUE_CAutomobile__TankControl = 0x6AF0FA;

static void HandleTankMissileHit(CVehicleSAInterface* pGameVehicle, CColPointSAInterface* pColPoint, CVector* pHitPosition, CEntitySAInterface** ppHitGameEntity)
{
    if (!pVehicleWeaponHitHandler)
        return;

    CEntitySAInterface* const pHitGameEntity = ppHitGameEntity ? *ppHitGameEntity : nullptr;
    const int                 iModel = pHitGameEntity ? pHitGameEntity->m_nModelIndex : -1;

    TriggerVehicleWeaponHitEvent(EVehicleWeaponType::TANK_GUN, pGameVehicle, pHitGameEntity, *pHitPosition, iModel, pColPoint->ucSurfaceTypeB);
}

static void _declspec(naked) HOOK_CAutomobile__TankControl()
{
    static intptr_t Fx_c__TriggerTankFire = 0x4A0FA0;

    _asm
    {
        call    Fx_c__TriggerTankFire
        pushad
        lea     eax, [esp + 0B4h - 2Ch] // CColPointSAInterface*
        lea     ebx, [esp + 0B4h - 58h] // CEntitySAInterface**
        lea     ecx, [esp + 0B4h - 70h] // CVector*
        push    ebx                     // ppHitGameEntity
        push    ecx                     // pHitPosition
        push    eax                     // pColPoint
        push    esi                     // pGameVehicle
        call    HandleTankMissileHit
        add     esp, 16
        popad
        jmp     CONTINUE_CAutomobile__TankControl
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CProjectileInfo::RemoveProjectile(CProjectileInfo*, CProjectile*)
//
// Creates an explosion for the projectile and deletes the CProjectile entity.
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x738AAC | 6A 00          | push    0
// >>> 0x738AAE | E8 9D DF FF FF | call    CExplosion::AddExplosion
//     0x738AB3 | 83 C4 28       | add     esp, 28h
//     0x738AB6 | 8B 4D 20       | mov     ecx, [ebp+20h]
#define HOOKPOS_CProjectileInfo__RemoveProjectile               0x738AAE
#define HOOKSIZE_CProjectileInfo__RemoveProjectile              5
static const DWORD CONTINUE_CProjectileInfo__RemoveProjectile = 0x738AB6;

static void __cdecl HandleProjectileExplosion(CEntitySAInterface* pOwner, CEntitySAInterface* pTarget, eExplosionType explosionType, CVector& vecPosition)
{
    if (pOwner->nType != ENTITY_TYPE_VEHICLE)
        return;

    EVehicleWeaponType weaponType = EVehicleWeaponType::INVALID;

    switch (explosionType)
    {
    case EXP_TYPE_ROCKET:
        weaponType = EVehicleWeaponType::ROCKET;
        break;
    case EXP_TYPE_ROCKET_WEAK:
        weaponType = EVehicleWeaponType::HEAT_SEEKING_ROCKET;
        break;
    default:
        return;
    }

    auto const pVehicle = reinterpret_cast<CVehicleSAInterface*>(pOwner);
    TriggerVehicleWeaponHitEvent(weaponType, pVehicle, pTarget, vecPosition, -1, -1);
}

static void _declspec(naked) HOOK_CProjectileInfo__RemoveProjectile()
{
    _asm
    {
        call    CMultiplayerSA::HOOKPOS_CExplosion_AddExplosion
        pushad
        lea     eax, [esp + 20h + 12]
        push    eax                         // vecPosition
        push    [esp + 24h + 8]             // explosionType
        push    [ebp + 8]                   // pTarget
        push    [ebp + 4]                   // pOwner
        call    HandleProjectileExplosion
        add     esp, 4*4
        popad
        add     esp, 28h
        jmp     CONTINUE_CProjectileInfo__RemoveProjectile
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::InitHooks_VehicleWeapons
//
// Setup hooks for vehicle weapon related events
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_VehicleWeapons()
{
    EZHookInstall(CWaterCannon__Render);
    EZHookInstall(CAutomobile__TankControl);
    EZHookInstall(CProjectileInfo__RemoveProjectile);
}
