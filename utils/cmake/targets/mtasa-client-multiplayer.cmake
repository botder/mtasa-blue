#
# Client multiplayer library target
#
set(CLIENT_MULTIPLAYER_DIR "${MTASA_CLIENT_DIR}/multiplayer_sa")

add_library(mtasa-client-multiplayer SHARED
    "${CLIENT_MULTIPLAYER_DIR}/CLimitsSA.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/CLimitsSA.h"
    "${CLIENT_MULTIPLAYER_DIR}/CMultiplayerSA.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/CMultiplayerSA.h"
    "${CLIENT_MULTIPLAYER_DIR}/CMultiplayerSA_1.3.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/CMultiplayerSA_ClothesCache.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/CMultiplayerSA_ClothesMemFix.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/CMultiplayerSA_ClothesSpeedUp.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/CMultiplayerSA_CrashFixHacks.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/CMultiplayerSA_CustomAnimations.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/CMultiplayerSA_Direct3D.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/CMultiplayerSA_Files.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/CMultiplayerSA_FixBadAnimId.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/CMultiplayerSA_FixLineOfSightArgs.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/CMultiplayerSA_HookDestructors.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/CMultiplayerSA_LicensePlate.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/CMultiplayerSA_ObjectLODSystem.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/CMultiplayerSA_Peds.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/CMultiplayerSA_Rendering.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/CMultiplayerSA_RwResources.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/CMultiplayerSA_Streaming.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/CMultiplayerSA_VehicleCollision.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/CMultiplayerSA_VehicleDamage.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/CMultiplayerSA_VehicleDummies.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/CMultiplayerSA_VehicleLights.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/CMultiplayerSA_Vehicles.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/CMultiplayerSA_VehicleWeapons.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/CMultiplayerSA_Weapons.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/COffsets.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/COffsets.h"
    "${CLIENT_MULTIPLAYER_DIR}/Common.h"
    "${CLIENT_MULTIPLAYER_DIR}/CPopulationSA.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/CPopulationSA.h"
    "${CLIENT_MULTIPLAYER_DIR}/CRemoteDataSA.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/CRemoteDataSA.h"
    "${CLIENT_MULTIPLAYER_DIR}/multiplayersa_init.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/multiplayersa_init.h"
    "${CLIENT_MULTIPLAYER_DIR}/multiplayer_hooksystem.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/multiplayer_hooksystem.h"
    "${CLIENT_MULTIPLAYER_DIR}/multiplayer_keysync.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/multiplayer_keysync.h"
    "${CLIENT_MULTIPLAYER_DIR}/multiplayer_shotsync.cpp"
    "${CLIENT_MULTIPLAYER_DIR}/multiplayer_shotsync.h"
    "${CLIENT_MULTIPLAYER_DIR}/StdInc.cpp" # for SharedUtil.hpp etc.
    "${CLIENT_MULTIPLAYER_DIR}/StdInc.h"
)

target_precompile_headers(mtasa-client-multiplayer PRIVATE "${CLIENT_MULTIPLAYER_DIR}/StdInc.h")

set_target_properties(mtasa-client-multiplayer PROPERTIES
    OUTPUT_NAME "multiplayer_sa"
    CXX_STANDARD 14
)

mtasa_set_target_outputdir(mtasa-client-multiplayer "mta")

target_link_libraries(mtasa-client-multiplayer PRIVATE
    mtasa-shared-sdk
    mtasa-client-sdk
    vendor-sparsehash
    vendor-hwbrk
)
