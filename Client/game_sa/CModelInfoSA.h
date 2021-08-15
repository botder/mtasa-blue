/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CModelInfoSA.h
 *  PURPOSE:     Header file for entity model information handler class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CModelInfo.h>
#include <game/Common.h>
#include "CRenderWareSA.h"

#define     RpGetFrame(__c)                 ((RwFrame*)(((RwObject *)(__c))->parent))

// #define     ARRAY_ModelLoaded               0x8E4CD0 // ##SA##
static void* CStreaming__ms_aInfoForModel = *(void**)(0x5B8B08 + 6);
static void* ARRAY_ModelLoaded = (char*)CStreaming__ms_aInfoForModel + 0x10;

#define     FUNC_CStreaming__HasModelLoaded 0x4044C0

// CModelInfo/ARRAY_ModelInfo __thiscall to load/replace vehicle models
#define     FUNC_LoadVehicleModel           0x4C95C0
#define     FUNC_LoadWeaponModel            0x4C9910
#define     FUNC_LoadPedModel               0x4C7340

#define     DWORD_AtomicsReplacerModelID    0xB71840
#define     FUNC_AtomicsReplacer            0x537150

// #define     ARRAY_ModelInfo                 0xA9B0C8
static void* ARRAY_ModelInfo = *(void**)(0x403DA4 + 3);
#define     CLASS_CText                     0xC1B340
#define     FUNC_CText_Get                  0x6A0050
#define     FUNC_GetModelFlags              0x4044E0
#define     FUNC_GetBoundingBox             0x4082F0

#define     FUNC_RemoveRef                  0x4C4BB0
#define     FUNC_IsBoatModel                0x4c5a70
#define     FUNC_IsCarModel                 0x4c5aa0
#define     FUNC_IsTrainModel               0x4c5ad0
#define     FUNC_IsHeliModel                0x4c5b00
#define     FUNC_IsPlaneModel               0x4c5b30
#define     FUNC_IsBikeModel                0x4c5b60
#define     FUNC_IsFakePlaneModel           0x4c5b90
#define     FUNC_IsMonsterTruckModel        0x4c5bc0
#define     FUNC_IsQuadBikeModel            0x4c5bf0
#define     FUNC_IsBmxModel                 0x4c5c20
#define     FUNC_IsTrailerModel             0x4c5c50
#define     FUNC_IsVehicleModelType         0x4c5c80

#define     FUNC_RemoveModel                0x4089a0
#define     FUNC_FlushRequestList           0x40E4E0

#define     FUNC_HasVehicleUpgradeLoaded    0x407820
#define     FUNC_RequestVehicleUpgrade      0x408C70

#define     FUNC_CVehicleModelInfo__GetNumRemaps        0x4C86B0
#define     FUNC_CVehicleStructure_delete   0x4C9580

#define     FUNC_AddPedModel                0x4c67a0
#define     VAR_CTempColModels_ModelPed1    0x968DF0

class CTimeInfoSAInterface;
class CBaseModelInfoSAInterface;
class CAtomicModelInfoSAInterface;
class CDamageAtomicModelInfoSAInterface;
class CLodAtomicModelInfoSAInterface;
class CClumpModelInfoSAInterface;

class CModelInfoSAInterface
{
public:
    // Use GetModelInfo(int index) to get model info by id
    static CBaseModelInfoSAInterface** ms_modelInfoPtrs;
    static CBaseModelInfoSAInterface*  GetModelInfo(int index) { return ms_modelInfoPtrs[index]; }
};

struct CBaseModelInfoSAInterface_vftable
{
    void*(__thiscall* ScalarDeletingDestructor)(CBaseModelInfoSAInterface*, std::int8_t);

    CAtomicModelInfoSAInterface*(__thiscall* AsAtomicModelInfoPtr)(CBaseModelInfoSAInterface*);

    CDamageAtomicModelInfoSAInterface*(__thiscall* AsDamageAtomicModelInfoPtr)(CBaseModelInfoSAInterface*);

    CLodAtomicModelInfoSAInterface*(__thiscall* AsLodAtomicModelInfoPtr)(CBaseModelInfoSAInterface*);

    // pure virtual in CBaseModelInfo
    std::uint8_t(__thiscall* GetModelType)(CBaseModelInfoSAInterface*);

    CTimeInfoSAInterface*(__thiscall* GetTimeInfo)(CBaseModelInfoSAInterface*);

    void(__thiscall* Init)(CBaseModelInfoSAInterface*);

    void(__thiscall* Shutdown)(CBaseModelInfoSAInterface*);

    // pure virtual in CBaseModelInfo
    void(__thiscall* DeleteRwObject)(CBaseModelInfoSAInterface*);

    // pure virtual in CBaseModelInfo
    std::int32_t(__thiscall* GetRwModelType)(CBaseModelInfoSAInterface*);

    // pure virtual in CBaseModelInfo
    RwObject*(__thiscall* CreateInstanceTagged)(CBaseModelInfoSAInterface*, RwMatrixTag*);

    // pure virtual in CBaseModelInfo
    RwObject*(__thiscall* CreateInstance)(CBaseModelInfoSAInterface*);

    void(__thiscall* SetAnimFile)(CBaseModelInfoSAInterface*, const char*);

    void(__thiscall* ConvertAnimFileIndex)(CBaseModelInfoSAInterface*);

    std::uint32_t(__thiscall* GetAnimFileIndex)(CBaseModelInfoSAInterface*);
};

struct CAtomicModelInfoSAInterface_vftable : CBaseModelInfoSAInterface_vftable
{
    void(__thiscall* SetAtomic)(CAtomicModelInfoSAInterface*, RpAtomic*);
};

struct CDamageAtomicModelInfoSAInterface_vftable : CAtomicModelInfoSAInterface_vftable
{
};

struct CLodAtomicModelInfoSAInterface_vftable : CAtomicModelInfoSAInterface_vftable
{
};

struct CTimeModelInfoSAInterface_vftable : CAtomicModelInfoSAInterface_vftable
{
};

struct CLodTimeModelInfoSAInterface_vftable : CLodAtomicModelInfoSAInterface_vftable
{
};

struct CClumpModelInfoSAInterface_vftable : CBaseModelInfoSAInterface_vftable
{
    void(__thiscall* SetAtomic)(CClumpModelInfoSAInterface*, RpAtomic*);
};

struct CWeaponModelInfoSAInterface_vftable : CClumpModelInfoSAInterface_vftable
{
};

struct CPedModelInfoSAInterface_vftable : CClumpModelInfoSAInterface_vftable
{
};

struct CVehicleModelInfoSAInterface_vftable : CClumpModelInfoSAInterface_vftable
{
};

class CBaseModelInfoSAInterface
{
public:
    union
    {
        std::uintptr_t                             m_vftableAddress;
        CBaseModelInfoSAInterface_vftable*         m_vftable;
        CAtomicModelInfoSAInterface_vftable*       m_vftableAtomic;
        CDamageAtomicModelInfoSAInterface_vftable* m_vftableDamageAtomic;
        CLodAtomicModelInfoSAInterface_vftable*    m_vftableLodAtomic;
        CTimeModelInfoSAInterface_vftable*         m_vftableTime;
        CLodTimeModelInfoSAInterface_vftable*      m_vftableLodTime;
        CClumpModelInfoSAInterface_vftable*        m_vftableClump;
        CWeaponModelInfoSAInterface_vftable*       m_vftableWeapon;
        CPedModelInfoSAInterface_vftable*          m_vftablePed;
        CVehicleModelInfoSAInterface_vftable*      m_vftableVehicle;
    };
    std::uint32_t m_hashKey;            // Generated by CKeyGen::GetUppercaseKey(char const *) called by CBaseModelInfo::SetModelName(char const *)
    std::uint16_t m_numRefs;
    std::uint16_t m_texDictionary;
    std::uint8_t  m_alpha;
    std::uint8_t  m_num2DEffects;
    std::uint16_t m_2DEffectIndex;
    std::uint16_t m_dynamicIndex;
    union
    {
        std::uint8_t m_baseFlags;
        struct
        {
            std::uint8_t m_hasBeenPreRendered : 1;
            std::uint8_t m_drawLast : 1;
            std::uint8_t m_drawAdditive : 1;
            std::uint8_t m_dontWriteZBuffer : 1;
            std::uint8_t m_dontCastShadowsOn : 1;
            std::uint8_t m_isLod : 1;
            std::uint8_t m_isBackFaceCulled : 1;
            std::uint8_t m_ownsColModel : 1;
        };
    };
    union
    {
        std::uint8_t m_otherFlags;
        union
        {
            struct // Flags from CClumpModelInfo
            {
                std::uint8_t m_isAnimated : 1;
                std::uint8_t m_hasComplexHierarchy : 1;
                std::uint8_t m_isDoor : 1;
                std::uint8_t m_wasCollisionStreamedWithModel : 1;
            };
            struct // Flags from CAtomicModelInfo
            {
                /* 1 */ std::uint8_t m_flag0x100: 1;    // CClumpModelInfo::SetIsAnimated
                /* 2 */ std::uint8_t m_flag0x200: 1;    // CClumpModelInfo::SetHasComplexHierarchy
                /* 3 */ std::uint8_t m_flag0x400 : 1;   // CClumpModelInfo::SetIsDoor
                /* 4 */ std::uint8_t m_flag0x800 : 1;   // CClumpModelInfo::SetCollisionWasStreamedWithModel
                /* 5 */ std::uint8_t m_flag0x1000 : 1;
                /* 6 */ std::uint8_t m_flag0x2000 : 1;
                /* 7 */ std::uint8_t m_flag0x4000 : 1;
                /* 8 */ std::uint8_t m_flag0x8000 : 1;
            };
            struct
            {
                std::uint8_t : 2;
                std::uint8_t m_vehicleUpgradeFlags : 6; // CAtomicModelInfo::SetupVehicleUpgradeFlags
            };
            #if 1
            struct
            {
                unsigned char dwUnknownFlag25 : 1;
                unsigned char dwUnknownFlag26 : 1;
                unsigned char dwUnknownFlag27 : 1;
                unsigned char bSwaysInWind : 1;
                unsigned char bCollisionWasStreamedWithModel : 1;            // CClumpModelInfo::SetCollisionWasStreamedWithModel(unsigned int)
                unsigned char bDontCollideWithFlyer : 1;                     // CAtomicModelInfo::SetDontCollideWithFlyer(unsigned int)
                unsigned char bHasComplexHierarchy : 1;                      // CClumpModelInfo::SetHasComplexHierarchy(unsigned int)
                unsigned char bWetRoadReflection : 1;                        // CAtomicModelInfo::SetWetRoadReflection(unsigned int)
            };
            #endif
        };
    };
    CColModelSAInterface* m_colModel;
    float                 m_unscaledLodDistance;
    union
    {
        RwObject* m_rwObject;
        RpClump*  m_rwClump;
        RpAtomic* m_rwAtomic;
    };

public:
    inline bool IsBaseModelInfo() const noexcept { return m_vftableAddress == 0x85BB9C; }
    inline bool IsAtomicModelInfo() const noexcept { return m_vftableAddress == 0x85BBF0; }
    inline bool IsDamageAtomicModelInfo() const noexcept { return m_vftableAddress == 0x85BC30; }
    inline bool IsLodAtomicModelInfo() const noexcept { return m_vftableAddress == 0x85BC70; }
    inline bool IsTimeModelInfo() const noexcept { return m_vftableAddress == 0x85BCB0; }
    inline bool IsLodTimeModelInfo() const noexcept { return m_vftableAddress == 0x85BCF0; }
    inline bool IsClumpModelInfo() const noexcept { return m_vftableAddress == 0x85BD30; }
    inline bool IsWeaponModelInfo() const noexcept { return m_vftableAddress == 0x85BD78; }
    inline bool IsPedModelInfo() const noexcept { return m_vftableAddress == 0x85BDC0; }
    inline bool IsVehicleModelInfo() const noexcept { return m_vftableAddress == 0x85C5C8; }
};
static_assert(sizeof(CBaseModelInfoSAInterface) == 0x20, "Invalid size for CBaseModelInfoSAInterface");

class CAtomicModelInfoSAInterface : public CBaseModelInfoSAInterface
{
public:
};
static_assert(sizeof(CAtomicModelInfoSAInterface) == 0x20, "Invalid size for CAtomicModelInfoSAInterface");

class CDamageAtomicModelInfoSAInterface : public CAtomicModelInfoSAInterface
{
public:
    RpAtomic* m_damagedAtomic;
};
static_assert(sizeof(CDamageAtomicModelInfoSAInterface) == 0x24, "Invalid size for CDamageAtomicModelInfoSAInterface");

class CLodAtomicModelInfoSAInterface : public CAtomicModelInfoSAInterface
{
public:
    std::uint16_t m_field0x20;
    std::uint16_t m_field0x22;
};
static_assert(sizeof(CLodAtomicModelInfoSAInterface) == 0x24, "Invalid size for CLodAtomicModelInfoSAInterface");

class CTimeInfoSAInterface
{
public:
    std::uint8_t  m_timeOn;
    std::uint8_t  m_timeOff;
    std::uint16_t m_otherTimeModel;
};
static_assert(sizeof(CTimeInfoSAInterface) == 0x4, "Invalid size for CTimeInfoSAInterface");

class CTimeModelInfoSAInterface : public CAtomicModelInfoSAInterface
{
public:
    CTimeInfoSAInterface m_timeInfo;
};
static_assert(sizeof(CTimeModelInfoSAInterface) == 0x24, "Invalid size for CTimeModelInfoSAInterface");

class CLodTimeModelInfoSAInterface : public CLodAtomicModelInfoSAInterface
{
public:
    CTimeInfoSAInterface m_timeInfo;
};
static_assert(sizeof(CLodTimeModelInfoSAInterface) == 0x28, "Invalid size for CLodTimeModelInfoSAInterface");

class CClumpModelInfoSAInterface : public CBaseModelInfoSAInterface
{
public:
    union
    {
        char*         m_animFile;
        std::uint32_t m_animBlockIndex; // Index to CAnimManager::ms_aAnimBlocks
    };
};
static_assert(sizeof(CClumpModelInfoSAInterface) == 0x24, "Invalid size for CClumpModelInfoSAInterface");

class CWeaponModelInfoSAInterface : public CClumpModelInfoSAInterface
{
public:
    std::uint32_t m_weaponInfo;
};
static_assert(sizeof(CWeaponModelInfoSAInterface) == 0x28, "Invalid size for CWeaponModelInfoSAInterface");

class CPedModelInfoSAInterface : public CClumpModelInfoSAInterface
{
public:
    std::uint32_t m_motionAnimGroup;
    ePedType      m_pedType;
    ePedStats     m_pedStatType;
    union
    {
        std::uint16_t m_carsCanDriveMask;
        struct
        {
            std::uint8_t m_drivesNormalCars : 1;
            std::uint8_t m_drivesPoorFamilyCars : 1;
            std::uint8_t m_drivesRichFamilyCars : 1;
            std::uint8_t m_drivesExecutiveCars : 1;
            std::uint8_t m_drivesWorkerCars : 1;
            std::uint8_t m_drivesBigCars : 1;
            std::uint8_t m_drivesTaxiCars : 1;
            std::uint8_t m_drivesMopeds : 1;

            std::uint8_t m_drivesMotorbikes : 1;
            std::uint8_t m_drivesLeisureBoats : 1;
            std::uint8_t m_drivesWorkerBoats : 1;
            std::uint8_t m_drivesBicycles : 1;
            std::uint8_t m_walksOnFoot : 1;
        };
    };
    union
    {
        std::uint16_t m_pedFlags;
        struct
        {
            std::uint8_t m_buysDrugs : 1;
        };
    };
    CColModelSAInterface* m_hitColModel;
    std::uint8_t          m_firstRadioStation;  // [0, 9]
    std::uint8_t          m_secondRadioStation; // [0, 9]
    std::uint8_t          m_race;
    std::uint16_t         m_audioPedType;       // PED_TYPE_GEN, ... => [0, 6]
    std::uint16_t         m_firstVoice;
    std::uint16_t         m_lastVoice;
    std::uint16_t         m_nextVoice;

public:
    CPedModelInfoSAInterface();
};
static_assert(sizeof(CPedModelInfoSAInterface) == 0x44, "Invalid size for CPedModelInfoSAInterface");

class CQuaternionSAInterface
{
public:
    CVector m_imaginary;
    float   m_real;
};
static_assert(sizeof(CQuaternionSAInterface) == 0x10, "Invalid size for CQuaternionSAInterface");

struct UpgradePosnDescSAInterface
{
    CVector                m_position;
    CQuaternionSAInterface m_rotation;
    std::uint32_t          m_parentID;
};
static_assert(sizeof(UpgradePosnDescSAInterface) == 0x20, "Invalid size for UpgradePosnDescSAInterface");

class CVehicleStructureSAInterface
{
public:
    static constexpr std::size_t NUM_DUMMIES = 15;
    static constexpr std::size_t NUM_UPGRADES = 18;
    static constexpr std::size_t NUM_EXTRAS = 6;

public:
    CVector                    m_dummyPositions[NUM_DUMMIES];
    UpgradePosnDescSAInterface m_upgrades[NUM_UPGRADES];
    RpAtomic*                  m_extras[NUM_EXTRAS];
    std::uint8_t               m_numExtras;
    std::uint32_t              m_damagableComponentsMask;
};
static_assert(sizeof(CVehicleStructureSAInterface) == 0x314, "Invalid size for CVehicleStructureSAInterface");

class CVehicleModelInfoSAInterface : public CClumpModelInfoSAInterface
{
public:
    RpMaterial*   m_customCarPlateMaterial;
    char          m_customCarPlateText[8];
    char          m_field0x30;
    std::uint8_t  m_customCarPlateDesign;
    char          m_gameName[8];
    std::uint32_t m_vehicleType;
    float         m_wheelSizeFront;
    float         m_wheelSizeRear;
    std::uint16_t m_wheelModelIndex;
    std::uint16_t m_handlingId;
    std::uint8_t  m_numDoors;
    std::uint8_t  m_vehicleClass;
    union
    {
        std::uint8_t m_flags;
        struct
        {
            std::uint8_t m_streamOutAfterFirstCarGen : 1;
            std::uint8_t m_internal : 1;
        };
    };
    std::uint8_t                  m_wheelUpgradeClass;
    std::uint8_t                  m_numTimesUsed;   // [0, 120]
    std::uint16_t                 m_frequency;
    std::uint32_t                 m_componentRulesMask;
    float                         m_bikeSteerAngle;
    CVehicleStructureSAInterface* m_vehicleStructure;
    std::uint8_t                  m_field0x60[464];
    RpMaterial*                   m_dirtMaterials[32];
    std::uint8_t                  m_primaryColors[8];
    std::uint8_t                  m_secondaryColors[8];
    std::uint8_t                  m_tertiaryColors[8];
    std::uint8_t                  m_quaternaryColors[8];
    std::uint8_t                  m_numPossibleColors;
    std::uint8_t                  m_lastColorVariation;
    std::uint8_t                  m_currentColor[4];
    std::uint16_t                 m_upgrades[18];
    std::uint16_t                 m_remapTexDictionaries[4];
    union
    {
        char*         m_vehicleAnimFile;
        std::uint32_t m_vehicleAnimBlockIndex;            // Index to CAnimManager::ms_aAnimBlocks
    };
};
static_assert(sizeof(CVehicleModelInfoSAInterface) == 0x308, "Invalid size for CVehicleModelInfoSAInterface");

/**
 * \todo Someone move GetLevelFromPosition out of here or delete it entirely please
 */

class CModelInfoSA : public CModelInfo
{
protected:
    CBaseModelInfoSAInterface*                                                   m_pInterface;
    DWORD                                                                        m_dwModelID;
    DWORD                                                                        m_dwParentID;
    DWORD                                                                        m_dwReferences;
    DWORD                                                                        m_dwPendingInterfaceRef;
    CColModel*                                                                   m_pCustomColModel;
    CColModelSAInterface*                                                        m_pOriginalColModelInterface;
    std::uint8_t                                                                 m_originalBaseFlags = 0;
    RpClump*                                                                     m_pCustomClump;
    static std::map<unsigned short, int>                                         ms_RestreamTxdIDMap;
    static std::map<DWORD, float>                                                ms_ModelDefaultLodDistanceMap;
    static std::map<DWORD, BYTE>                                                 ms_ModelDefaultAlphaTransparencyMap;
    static std::unordered_map<std::uint32_t, std::map<eVehicleDummies, CVector>> ms_ModelDefaultDummiesPosition;
    static std::map<CTimeInfoSAInterface*, CTimeInfoSAInterface*>                ms_ModelDefaultModelTimeInfo;
    static std::unordered_map<DWORD, unsigned short>                             ms_OriginalObjectPropertiesGroups;
    static std::unordered_map<DWORD, std::pair<float, float>>                    ms_VehicleModelDefaultWheelSizes;
    bool                                                                         m_bAddedRefForCollision;
    SVehicleSupportedUpgrades                                                    m_ModelSupportedUpgrades;

public:
    CModelInfoSA();

    CBaseModelInfoSAInterface* GetInterface();
    CPedModelInfoSAInterface*  GetPedModelInfoInterface() { return reinterpret_cast<CPedModelInfoSAInterface*>(GetInterface()); }

    DWORD          GetModel() { return m_dwModelID; }
    eModelInfoType GetModelType();
    uint           GetAnimFileIndex();

    bool IsPlayerModel();

    BOOL IsBoat();
    BOOL IsCar();
    BOOL IsTrain();
    BOOL IsHeli();
    BOOL IsPlane();
    BOOL IsBike();
    BOOL IsFakePlane();
    BOOL IsMonsterTruck();
    BOOL IsQuadBike();
    BOOL IsBmx();
    BOOL IsTrailer();
    bool IsVehicle() const override;
    BOOL IsUpgrade();

    char* GetNameIfVehicle();

    BYTE           GetVehicleType();
    VOID           Request(EModelRequestType requestType, const char* szTag);
    VOID           Remove();
    BYTE           GetLevelFromPosition(CVector* vecPosition);
    BOOL           IsLoaded();
    BOOL           DoIsLoaded();
    BYTE           GetFlags();
    CBoundingBox*  GetBoundingBox();
    bool           IsValid();
    bool           IsAllocatedInArchive();
    float          GetDistanceFromCentreOfMassToBaseOfModel();
    unsigned short GetTextureDictionaryID();
    void           SetTextureDictionaryID(unsigned short usID);
    float          GetLODDistance();
    float          GetOriginalLODDistance();
    void           SetLODDistance(float fDistance, bool bOverrideMaxDistance = false);
    static void    StaticResetLodDistances();
    void           RestreamIPL();
    static void    StaticFlushPendingRestreamIPL();
    static void    StaticSetHooks();
    bool           GetTime(char& cHourOn, char& cHourOff);
    bool           SetTime(char cHourOn, char cHourOff);
    static void    StaticResetModelTimes();

    void        SetAlphaTransparencyEnabled(BOOL bEnabled);
    bool        IsAlphaTransparencyEnabled();
    void        ResetAlphaTransparency();
    static void StaticResetAlphaTransparencies();

    void ModelAddRef(EModelRequestType requestType, const char* szTag);
    int  GetRefCount();
    void RemoveRef(bool bRemoveExtraGTARef = false);
    bool ForceUnload();

    // CVehicleModelInfo specific
    short        GetAvailableVehicleMod(unsigned short usSlot);
    bool         IsUpgradeAvailable(eVehicleUpgradePosn posn);
    void         SetCustomCarPlateText(const char* szText);
    unsigned int GetNumRemaps();
    void*        GetVehicleSuspensionData();
    void*        SetVehicleSuspensionData(void* pSuspensionLines);
    CVector      GetVehicleExhaustFumesPosition() override;
    void         SetVehicleExhaustFumesPosition(const CVector& vecPosition) override;
    CVector      GetVehicleDummyDefaultPosition(eVehicleDummies eDummy) override;
    CVector      GetVehicleDummyPosition(eVehicleDummies eDummy) override;
    bool         GetVehicleDummyPositions(std::array<CVector, VEHICLE_DUMMY_COUNT>& positions) const override;
    void         SetVehicleDummyPosition(eVehicleDummies eDummy, const CVector& vecPosition) override;
    void         ResetVehicleDummies(bool bRemoveFromDummiesMap);
    static void  ResetAllVehicleDummies();
    float        GetVehicleWheelSize(eResizableVehicleWheelGroup eWheelGroup) override;
    void         SetVehicleWheelSize(eResizableVehicleWheelGroup eWheelGroup, float fWheelSize) override;
    void         ResetVehicleWheelSizes(std::pair<float, float>* defaultSizes = nullptr) override;
    static void  ResetAllVehiclesWheelSizes();

    // ONLY use for peds
    void GetVoice(short* psVoiceType, short* psVoice);
    void GetVoice(const char** pszVoiceType, const char** szVoice);
    void SetVoice(short sVoiceType, short sVoice);
    void SetVoice(const char* szVoiceType, const char* szVoice);

    // Custom collision related functions
    void SetCustomModel(RpClump* pClump) override;
    void RestoreOriginalModel() override;
    void SetColModel(CColModel* pColModel) override;
    void RestoreColModel() override;
    void MakeCustomModel() override;

    // Increases the collision slot reference counter for the original collision model
    void AddColRef() override;

    // Decreases the collision slot reference counter for the original collision model
    void RemoveColRef() override;

    void SetModelID(DWORD dwModelID) { m_dwModelID = dwModelID; }

    RwObject* GetRwObject() { return m_pInterface ? m_pInterface->m_rwObject : NULL; }

    // CModelInfoSA methods
    void MakePedModel(char* szTexture);
    void MakeObjectModel(ushort usBaseModelID);
    void MakeVehicleAutomobile(ushort usBaseModelID);
    void DeallocateModel(void);
    unsigned int GetParentID() { return m_dwParentID; };

    SVehicleSupportedUpgrades GetVehicleSupportedUpgrades() { return m_ModelSupportedUpgrades; }

    void InitialiseSupportedUpgrades(RpClump* pClump);
    void ResetSupportedUpgrades();

    void           SetObjectPropertiesGroup(unsigned short usObjectGroup);
    unsigned short GetObjectPropertiesGroup();
    void           RestoreObjectPropertiesGroup();
    static void    RestoreAllObjectsPropertiesGroups();

    // Vehicle towing functions
    bool IsTowableBy(CModelInfo* towingModel) override;

private:
    void CopyStreamingInfoFromModel(ushort usCopyFromModelID);
    void RwSetSupportedUpgrades(RwFrame* parent, DWORD dwModel);
};
