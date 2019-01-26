/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_VehicleDamage.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

// Used by tyre burst
extern CPedSAInterface*    pBulletImpactInitiator;
extern CEntitySAInterface* pBulletImpactVictim;
extern CVector             vecSavedBulletImpactEndPosition;

namespace
{
    // Used to save info between CVehicle_VehicleDamage1 and CVehicle_VehicleDamage2
    bool                ms_HasSavedData = false;
    eWeaponType         ms_SavedWeaponType;
    CEntitySAInterface* ms_SavedAttacker;
    CVector             ms_SavedDamagedPos;

    VehicleDamageHandler* m_pVehicleDamageHandler = NULL;

    CVehicleSAInterface* pHookVehicle = nullptr;
}            // namespace

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVehicle::BurstTyre
//      (Used for CAutomobile and CBike hooks)
//
// Called when an inflated vehicle tyre is hit by a bullet
//
//////////////////////////////////////////////////////////////////////////////////////////
bool OnMY_CVehicle_BurstTyre(uchar ucTyre)
{
    OUTPUT_VTABLE_DEBUG("PRE", pHookVehicle);

    if (pHookVehicle && m_pVehicleDamageHandler)
    {
        eWeaponType weaponType = WEAPONTYPE_INVALID;

        // Discover weapon if possible
        SClientEntity<CPedSA>* pPedClientEntity = pGameInterface->GetPools()->GetPed((DWORD*)pBulletImpactInitiator);
        CPed*                  pInitiator = pPedClientEntity ? pPedClientEntity->pEntity : nullptr;
        if (pInitiator)
        {
            CWeapon* pWeapon = pInitiator->GetWeapon(pInitiator->GetCurrentWeaponSlot());
            if (pWeapon)
                weaponType = pWeapon->GetType();
        }

        if (!m_pVehicleDamageHandler(pHookVehicle, 0, pBulletImpactInitiator, weaponType, vecSavedBulletImpactEndPosition, ucTyre))
            return false;
    }
    
    OUTPUT_VTABLE_DEBUG("POST", pHookVehicle);
    return true;
}

// Hook info
#define HOOKPOS_CAutomobile_BurstTyre                       0x06A331C
#define HOOKSIZE_CAutomobile_BurstTyre                      5
#define HOOKCHECK_CAutomobile_BurstTyre                     0x6A
DWORD RETURN_CAutomobile_BurstTyre_A = 0x06A3321;
DWORD RETURN_CAutomobile_BurstTyre_B = 0x06A3425;
void _declspec(naked) HOOK_CAutomobile_BurstTyre()
{
    _asm
    {
        pushad
        mov     pHookVehicle, esi
        push    edi
        call    OnMY_CVehicle_BurstTyre
        add     esp, 4*1

        test    al, al
        jnz     cont

        popad
        mov     esi, pHookVehicle
        jmp     RETURN_CAutomobile_BurstTyre_B
cont:

        popad
        mov     esi, pHookVehicle
        push    1
        push    edi
        mov     ecx, ebx
        jmp     RETURN_CAutomobile_BurstTyre_A
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CBike::BurstTyre
//
//
//////////////////////////////////////////////////////////////////////////////////////////
// Hook info
#define HOOKPOS_CBike_BurstTyre                         0x06BEB94
#define HOOKSIZE_CBike_BurstTyre                        10
#define HOOKCHECK_CBike_BurstTyre                       0x6A
DWORD RETURN_CBike_BurstTyre_A = 0x06BEB9E;
DWORD RETURN_CBike_BurstTyre_B = 0x06BECA5;
void _declspec(naked) HOOK_CBike_BurstTyre()
{
    _asm
    {
        pushad
        mov     pHookVehicle, esi
        movzx   eax, bl
        push    eax
        call    OnMY_CVehicle_BurstTyre
        add     esp, 4*1

        test    al, al
        jnz     cont

        popad
        mov     esi, pHookVehicle
        jmp     RETURN_CBike_BurstTyre_B
cont:

        popad
        mov     esi, pHookVehicle
        push    0
        push    5Ah
        lea     ecx, [esi+138h]
        jmp     RETURN_CBike_BurstTyre_A
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVehicle::InflictDamage
//
// Called when a bullet, projectile or flame causes the vehicle damage
//
//////////////////////////////////////////////////////////////////////////////////////////
bool OnMY_CVehicle_InflictDamage(CEntitySAInterface* pAttacker, eWeaponType weaponType, float fDamage, CVector vecDamagePos)
{
    OUTPUT_VTABLE_DEBUG("PRE", pHookVehicle);

    if (pHookVehicle && m_pVehicleDamageHandler)
    {
        if (!m_pVehicleDamageHandler(pHookVehicle, fDamage, pAttacker, weaponType, vecDamagePos, UCHAR_INVALID_INDEX))
            return false;
    }

    OUTPUT_VTABLE_DEBUG("POST", pHookVehicle);
    return true;
}

// Hook info
#define HOOKPOS_CVehicle_InflictDamage_US                       0x06D7C90
#define HOOKSIZE_CVehicle_InflictDamage_US                      5
#define HOOKCHECK_CVehicle_InflictDamage_US                     0x6A
#define HOOKPOS_CVehicle_InflictDamage_EU                       0x06D7C90
#define HOOKSIZE_CVehicle_InflictDamage_EU                      5
#define HOOKCHECK_CVehicle_InflictDamage_EU                     0x6A
DWORD RETURN_CVehicle_InflictDamage_US = 0x0404CDC;
DWORD RETURN_CVehicle_InflictDamage_EU = 0x0404CE3;
DWORD RETURN_CVehicle_InflictDamage_BOTH = 0;
void _declspec(naked) HOOK_CVehicle_InflictDamage()
{
    _asm
    {
        pushad
        mov     pHookVehicle, esi
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        call    OnMY_CVehicle_InflictDamage
        add     esp, 4*6

        test    al, al
        jnz     cont

        popad
        mov     esi, pHookVehicle
        retn    18h
cont:

        popad
        mov     esi, pHookVehicle
        push    0FFFFFFFFh
        jmp     RETURN_CVehicle_InflictDamage_BOTH
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVehicle::VehicleDamage hook 1
//      (Used for CAutomobile, CBike and CPlane hooks)
//
// Called when a vehicle hits (or is hit by) another vehicle/object/ped
// Save attacker and weapon
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_CVehicle_VehicleDamage1(CVehicleSAInterface* pVehicle, float f, ushort a, CEntitySAInterface* pAttacker, CVector* pvecPosition,
                                  CVector* pvecSomething, eWeaponType weaponType)
{
    if (!pvecPosition || !f)
        return;

    ms_HasSavedData = true;
    ms_SavedAttacker = pAttacker;
    ms_SavedWeaponType = weaponType;
    ms_SavedDamagedPos = *pvecPosition;
}

// Hook info
#define HOOKPOS_CAutomobile_VehicleDamage1                      0x06A7650
#define HOOKSIZE_CAutomobile_VehicleDamage1                     7
#define HOOKCHECK_CAutomobile_VehicleDamage1                    0x6A
DWORD RETURN_CAutomobile_VehicleDamage1 = 0x06A7657;
void _declspec(naked) HOOK_CAutomobile_VehicleDamage1()
{
    _asm
    {
        pushad
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    ecx
        call    OnMY_CVehicle_VehicleDamage1
        add     esp, 4*6+4
        popad

        // Continue replaced code
        push    0FFFFFFFFh
        push    [0x847FD8]
        jmp     RETURN_CAutomobile_VehicleDamage1
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVehicle::VehicleDamage hook 2
//      (Used for CAutomobile, CBike and CPlane hooks)
//
// Trigger event
//
//////////////////////////////////////////////////////////////////////////////////////////
float OnMY_CVehicle_VehicleDamage2(float fDamage)
{
    OUTPUT_VTABLE_DEBUG("PRE", pHookVehicle);

    if (pHookVehicle && m_pVehicleDamageHandler)
    {
        eWeaponType         weaponType = WEAPONTYPE_INVALID;
        CEntitySAInterface* pAttacker = pHookVehicle->m_pCollidedEntity;
        CVector             vecDamagePos = pHookVehicle->m_vecCollisionPosition;

        if (ms_HasSavedData)
        {
            if (ms_SavedAttacker)
                pAttacker = ms_SavedAttacker;
            weaponType = ms_SavedWeaponType;
            vecDamagePos = ms_SavedDamagedPos;
        }

        if (!m_pVehicleDamageHandler(pHookVehicle, fDamage, pAttacker, weaponType, vecDamagePos, UCHAR_INVALID_INDEX))
            fDamage = 0;
    }

    OUTPUT_VTABLE_DEBUG("POST", pHookVehicle);
    ms_HasSavedData = false;
    return fDamage;
}

// Hook info
#define HOOKPOS_CAutomobile_VehicleDamage2                      0x06A8325
#define HOOKSIZE_CAutomobile_VehicleDamage2                     6
#define HOOKCHECK_CAutomobile_VehicleDamage2                    0xD8
DWORD RETURN_CAutomobile_VehicleDamage2 = 0x06A832B;
void _declspec(naked) HOOK_CAutomobile_VehicleDamage2()
{
    _asm
    {
        pushad
        mov     pHookVehicle, esi
        fstp    [esp-4] // Pop loss
        push    [esp-4]
        call    OnMY_CVehicle_VehicleDamage2
        add     esp, 4*1
        popad
        mov     esi, pHookVehicle

        // Loss is on fp stack (from function return)
        // Continue replaced code
        fsubr   dword ptr [esi+4C0h]
        jmp     RETURN_CAutomobile_VehicleDamage2
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CPlane::VehicleDamage hook 1
//
// Save attacker and weapon
//
//////////////////////////////////////////////////////////////////////////////////////////
// Hook info
#define HOOKPOS_CPlane_VehicleDamage1                       0x06CC4B0
#define HOOKSIZE_CPlane_VehicleDamage1                      8
#define HOOKCHECK_CPlane_VehicleDamage1                     0x83
DWORD RETURN_CPlane_VehicleDamage1 = 0x06CC4B8;
void _declspec(naked) HOOK_CPlane_VehicleDamage1()
{
    _asm
    {
        pushad
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    ecx
        call    OnMY_CVehicle_VehicleDamage1
        add     esp, 4*6+4
        popad

        // Continue replaced code
        sub     esp, 48h
        mov     eax, dword ptr ds:[0x08D33E4]
        jmp     RETURN_CPlane_VehicleDamage1
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CPlane::VehicleDamage hook 2
//
// Trigger event
//
//////////////////////////////////////////////////////////////////////////////////////////
// Hook info
#define HOOKPOS_CPlane_VehicleDamage2                       0x06CC6C8
#define HOOKSIZE_CPlane_VehicleDamage2                      6
#define HOOKCHECK_CPlane_VehicleDamage2                     0xD8
DWORD RETURN_CPlane_VehicleDamage2 = 0x06CC6CE;
void _declspec(naked) HOOK_CPlane_VehicleDamage2()
{
    _asm
    {
        pushad
        mov     pHookVehicle, esi
        fstp    [esp-4] // Pop loss
        push    [esp-4]
        call    OnMY_CVehicle_VehicleDamage2
        add     esp, 4*1
        popad
        mov     esi, pHookVehicle

        // Loss is on fp stack (from function return)
        // Continue replaced code
        fsubr   dword ptr [esi+4C0h]
        jmp     RETURN_CPlane_VehicleDamage2
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CBike::VehicleDamage hook 1
//
// Save attacker and weapon
//
//////////////////////////////////////////////////////////////////////////////////////////
// Hook info
#define HOOKPOS_CBike_VehicleDamage1                        0x06B8EC0
#define HOOKSIZE_CBike_VehicleDamage1                       5
#define HOOKCHECK_CBike_VehicleDamage1                      0x51
DWORD RETURN_CBike_VehicleDamage1 = 0x06B8EC5;
void _declspec(naked) HOOK_CBike_VehicleDamage1()
{
    _asm
    {
        pushad
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    ecx
        call    OnMY_CVehicle_VehicleDamage1
        add     esp, 4*6+4
        popad

        // Continue replaced code
        push    ecx
        fld     [esp+8]
        jmp     RETURN_CBike_VehicleDamage1
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CBike::VehicleDamage hook 2
//
// Trigger event
//
//////////////////////////////////////////////////////////////////////////////////////////
// Hook info
#define HOOKPOS_CBike_VehicleDamage2                        0x06B91C2
#define HOOKSIZE_CBike_VehicleDamage2                       6
#define HOOKCHECK_CBike_VehicleDamage2                      0xD8
DWORD RETURN_CBike_VehicleDamage2 = 0x06B91C8;
void _declspec(naked) HOOK_CBike_VehicleDamage2()
{
    _asm
    {
        pushad
        mov     pHookVehicle, esi
        fstp    [esp-4] // Pop loss
        push    [esp-4]
        call    OnMY_CVehicle_VehicleDamage2
        add     esp, 4*1
        popad
        mov     esi, pHookVehicle

        // Loss is on fp stack (from function return)
        // Continue replaced code
        fsubr   dword ptr [esi+4C0h]
        jmp     RETURN_CBike_VehicleDamage2
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CPlaceable::~CPlaceable()
//
// Destructor for CPlaceable. Replace the vftable pointer with nullptr
// to make debugging easier
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0054F496 | 85 C0             | test  eax, eax
// >>> 0054F498 | C7 06 40 3C 86 00 | mov   dword ptr [esi], offset CPlaceable::vftable
//     0054F49E | 74 0B             | jz    short loc_54F4AB
#define HOOKPOS_CPlaceable_Destructor         0x54F498
#define HOOKSIZE_CPlaceable_Destructor        6
#define HOOKCHECK_CPlaceable_Destructor       0xC7
static DWORD CONTINUE_CPlaceable_Destructor = 0x54F49E;

static void Print_CPlaceable_Destructor_DebugInfo()
{
    // OUTPUT_VTABLE_DEBUG("INFO", pHookVehicle);
}

static void _declspec(naked) HOOK_CPlaceable_Destructor()
{
    _asm
    {
        // pushad
        // mov     pHookVehicle, esi
        // call    Print_CPlaceable_Destructor_DebugInfo
        // popad
        mov     dword ptr [esi], 0
        jmp     CONTINUE_CPlaceable_Destructor
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::SetVehicleDamageHandler
//
// Set handler for functions in this file
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::SetVehicleDamageHandler(VehicleDamageHandler* pHandler)
{
    m_pVehicleDamageHandler = pHandler;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::InitHooks_VehicleDamage
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_VehicleDamage()
{
    EZHookInstallChecked(CVehicle_InflictDamage);
    EZHookInstallChecked(CAutomobile_BurstTyre);
    EZHookInstallChecked(CBike_BurstTyre);
    EZHookInstallChecked(CAutomobile_VehicleDamage1);
    EZHookInstallChecked(CAutomobile_VehicleDamage2);
    EZHookInstallChecked(CPlane_VehicleDamage1);
    EZHookInstallChecked(CPlane_VehicleDamage2);
    EZHookInstallChecked(CBike_VehicleDamage1);
    EZHookInstallChecked(CBike_VehicleDamage2);
#ifdef MTA_DEBUG
    EZHookInstallChecked(CPlaceable_Destructor);
#endif
}
