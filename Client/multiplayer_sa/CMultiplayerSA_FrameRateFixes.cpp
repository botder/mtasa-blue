/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_FrameRateFixes.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <../game_sa/CEntitySA.h>

extern CCoreInterface* g_pCore;

constexpr float kOriginalTimeStep = 50.0f / 30.0f;

#define HOOKPOS_CTaskSimpleUseGun__SetMoveAnim 0x61E4F2
#define HOOKSIZE_CTaskSimpleUseGun__SetMoveAnim 0x6
const unsigned int RETURN_CTaskSimpleUseGun__SetMoveAnim = 0x61E4F8;

void _declspec(naked) HOOK_CTaskSimpleUseGun__SetMoveAnim()
{
    _asm {
        fld ds:[0xB7CB5C]           // CTimer::ms_fTimeStep
        fdiv kOriginalTimeStep      // 1.666f
        fmul ds:[0x858B1C]          // 0.1f
        fxch
        fcom
        fxch
        fstp st(0)
        jmp RETURN_CTaskSimpleUseGun__SetMoveAnim
    }
}

namespace
{
    // Original simulation time in milliseconds in GTA:SA
    constexpr float SIMULATION_TIME_IN_MS = 1000.0f / 30.0f;

    // Original simulation time maximum in milliseconds in GTA:SA
    constexpr float MAX_SIMULATION_TIME_IN_MS = 300.0f;

    // Original simulation time step in GTA:SA
    constexpr float SIMULATION_TIME_STEP = SIMULATION_TIME_IN_MS / 20.0f;
}

#define REF_FROM_GAME(variable, address) \
    decltype(variable)(variable) = *reinterpret_cast<typename std::add_pointer<typename std::remove_reference<decltype(variable)>::type>::type>(address)

#define PTR_FROM_GAME(variable, address) \
    decltype(variable)(variable) = reinterpret_cast<typename std::remove_reference<decltype(variable)>::type>(address)

static auto CEntity__UpdateRwFrame = reinterpret_cast<void(__fastcall*)(CEntitySAInterface*)>(0x532B00);

namespace gtasa
{
    class CPtrNodeDoubleLink
    {
    public:
        CEntitySAInterface* m_item{nullptr};
        CPtrNodeDoubleLink* m_next{nullptr};
        CPtrNodeDoubleLink* m_previous{nullptr};
    };

    template <bool Mutable>
    class CPtrListIteratorDoubleLink
    {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = typename std::conditional<Mutable, CEntitySAInterface, const CEntitySAInterface>::type;
        using reference = value_type&;
        using pointer = value_type*;

    public:
        CPtrListIteratorDoubleLink() noexcept = default;

        CPtrListIteratorDoubleLink(CPtrNodeDoubleLink* node) noexcept : m_node(node) {}

        void swap(CPtrListIteratorDoubleLink& other) noexcept { std::swap(m_node, other.m_node); }

    public:
        pointer operator->() const { return m_node->m_item; }

        reference operator*() const { return *m_node->m_item; }

        CPtrListIteratorDoubleLink& operator++() noexcept
        {
            if (m_node != nullptr)
            {
                m_node = m_node->m_next;
            }

            return *this;
        }

        CPtrListIteratorDoubleLink& operator++(int) const noexcept
        {
            CPtrListIteratorDoubleLink temp = *this;
            ++(*this);
            return temp;
        }

        CPtrListIteratorDoubleLink& operator--() noexcept
        {
            if (m_node != nullptr)
            {
                m_node = m_node->m_previous;
            }

            return *this;
        }

        CPtrListIteratorDoubleLink& operator--(int) const noexcept
        {
            CPtrListIteratorDoubleLink temp = *this;
            --(*this);
            return temp;
        }

    public:
        bool operator==(const CPtrListIteratorDoubleLink& other) const noexcept
        {
            if (m_node != nullptr && other.m_node != nullptr)
            {
                return m_node->m_item == other.m_node->m_item;
            }

            return m_node == nullptr && other.m_node == nullptr;
        }

        bool operator!=(const CPtrListIteratorDoubleLink& other) const noexcept { return !(*this == other); }

    private:
        CPtrNodeDoubleLink* m_node{nullptr};
    };

    class CPtrListDoubleLink
    {
    public:
        using iterator = CPtrListIteratorDoubleLink<true>;
        using const_iterator = CPtrListIteratorDoubleLink<false>;

        iterator begin() { return iterator(m_head); }
        iterator end() { return iterator(nullptr); }

        const_iterator begin() const { return const_iterator(m_head); }
        const_iterator end() const { return const_iterator(nullptr); }

    public:
        CPtrNodeDoubleLink* m_head{nullptr};
    };
}

namespace gtasa
{
    class CWorld
    {
    public:
        static void ProcessProxy();

    public:
        static void(__cdecl* Process)();

        static void(__cdecl* ProcessAttachedEntities)();

    public:
        static CPtrListDoubleLink& ms_listMovingEntityPtrs;
    };

    PTR_FROM_GAME(CWorld::Process, 0x5684A0);
    PTR_FROM_GAME(CWorld::ProcessAttachedEntities, 0x5647F0);

    REF_FROM_GAME(CWorld::ms_listMovingEntityPtrs, 0xB9ACC8);
}

namespace gtasa
{
    class CSpecialFX
    {
    public:
        static bool& ms_isSnapShotActive;
    };

    REF_FROM_GAME(CSpecialFX::ms_isSnapShotActive, 0xC7C714);
}

namespace gtasa
{
    using TimerFunction = uint64_t(__cdecl*)();

    class CTimer
    {
    public:
        static void InitialiseProxy();

        static void UpdateVariablesProxy(float deltaCycles);

        static void UpdateProxy();

    public:
        static void(__cdecl* Initialise)();

        static void(__cdecl* Update)();

        static void(__cdecl* UpdateVariables)(float deltaCycles);

    public:
        static bool&          ms_isRunning;
        static TimerFunction& ms_timer;
        static uint32_t&      ms_cyclesPerMillisecond;
        static uint64_t&      ms_lastUpdateTimePoint;
        static bool&          ms_skipProcessThisFrame;
        static float&         ms_FPS;
        static uint32_t&      ms_frameCounter;
        static float&         ms_timeScale;
        static float&         ms_timeStep;
        static float&         ms_timeStepNonClipped;
        static float&         ms_previousTimeStep;
        static bool&          ms_codePause;
        static bool&          ms_userPause;
        static uint32_t&      ms_timeInMilliseconds;
        static uint32_t&      ms_timeInMillisecondsPauseMode;
        static uint32_t&      ms_timeInMillisecondsNonClipped;
        static uint32_t&      ms_previousTimeInMilliseconds;
        static uint32_t&      ms_previousTimeInMillisecondsNonClipped;
        static uint32_t&      ms_PPreviousTimeInMilliseconds;
        static uint32_t&      ms_PPPreviousTimeInMilliseconds;
        static uint32_t&      ms_PPPPreviousTimeInMilliseconds;
    };

    PTR_FROM_GAME(CTimer::Initialise, 0x5617E0);
    PTR_FROM_GAME(CTimer::Update, 0x561B10);
    PTR_FROM_GAME(CTimer::UpdateVariables, 0x5618D0);

    REF_FROM_GAME(CTimer::ms_isRunning, 0xB7CB40);
    REF_FROM_GAME(CTimer::ms_timer, 0xB7CB28);
    REF_FROM_GAME(CTimer::ms_cyclesPerMillisecond, 0xB7CB2C);
    REF_FROM_GAME(CTimer::ms_lastUpdateTimePoint, 0xB7CB38);
    REF_FROM_GAME(CTimer::ms_skipProcessThisFrame, 0xB7CB89);
    REF_FROM_GAME(CTimer::ms_FPS, 0xB7CB50);
    REF_FROM_GAME(CTimer::ms_frameCounter, 0xB7CB4C);
    REF_FROM_GAME(CTimer::ms_timeScale, 0xB7CB64);
    REF_FROM_GAME(CTimer::ms_timeStep, 0xB7CB5C);
    REF_FROM_GAME(CTimer::ms_timeStepNonClipped, 0xB7CB58);
    REF_FROM_GAME(CTimer::ms_previousTimeStep, 0xB7CB54);
    REF_FROM_GAME(CTimer::ms_codePause, 0xB7CB48);
    REF_FROM_GAME(CTimer::ms_userPause, 0xB7CB49);
    REF_FROM_GAME(CTimer::ms_timeInMilliseconds, 0xB7CB84);
    REF_FROM_GAME(CTimer::ms_timeInMillisecondsPauseMode, 0xB7CB7C);
    REF_FROM_GAME(CTimer::ms_timeInMillisecondsNonClipped, 0xB7CB80);
    REF_FROM_GAME(CTimer::ms_previousTimeInMilliseconds, 0xB7CB78);
    REF_FROM_GAME(CTimer::ms_previousTimeInMillisecondsNonClipped, 0xB7CB68);
    REF_FROM_GAME(CTimer::ms_PPreviousTimeInMilliseconds, 0xB7CB74);
    REF_FROM_GAME(CTimer::ms_PPPreviousTimeInMilliseconds, 0xB7CB70);
    REF_FROM_GAME(CTimer::ms_PPPPreviousTimeInMilliseconds, 0xB7CB6C);
}

namespace gtasa
{
    class CSkidmarks
    {
    public:
        static void UpdateProxy();

    public:
        static void(__cdecl* Update)();
    };

    PTR_FROM_GAME(CSkidmarks::Update, 0x7205C0);
}

namespace gtasa
{
    class CPhysical
    {
    public:
        static void __fastcall AddToMovingListProxy(CPhysicalSAInterface* self);

        static void __fastcall RemoveFromMovingListProxy(CPhysicalSAInterface* self);

    public:
        static void(__fastcall* AddToMovingList)(CPhysicalSAInterface* self);

        static void(__fastcall* RemoveFromMovingList)(CPhysicalSAInterface* self);
    };

    PTR_FROM_GAME(CPhysical::AddToMovingList, 0x5684A0);
    PTR_FROM_GAME(CPhysical::RemoveFromMovingList, 0x5684A0);
}

namespace
{
    // Simulation time in cycles (depends on timer granularity; initialized after CTimer::Initialise)
    float SIMULATION_TIME_IN_CYCLES = 0.0;

    // Simulation time in cycles (depends on timer granularity; initialized after CTimer::Initialise)
    float MAX_SIMULATION_TIME_IN_CYCLES = 0.0;
}

static void AddCrossProduct(CVector& target, const CVector& other)
{
    CVector copy = other;
    copy.CrossProduct(&target);
    target += copy;
}

namespace mtasa
{
    class EntitySnapshot
    {
    public:
        EntitySnapshot() noexcept = default;

        explicit EntitySnapshot(CPhysicalSAInterface* entity) { Capture(entity); }

        void Capture(CPhysicalSAInterface* entity)
        {
            m_entity = entity;

            if (entity == nullptr)
                return;

            if (entity->Placeable.matrix != nullptr)
            {
                entity->Placeable.matrix->ConvertToMatrix(m_matrix);
            }
            else
            {
                m_matrix.SetPosition(entity->Placeable.m_transform.m_translate);
                m_matrix.SetRotation(CVector(0, 0, entity->Placeable.m_transform.m_heading));
            }

            m_linearVelocity = entity->m_vecLinearVelocity;
            m_angularVelocity = entity->m_vecAngularVelocity;
        }

        void Restore() const
        {
            if (m_entity == nullptr)
                return;

            if (m_entity->Placeable.matrix != nullptr)
            {
                m_entity->Placeable.matrix->SetFromMatrix(m_matrix);
            }
            else
            {
                m_entity->Placeable.m_transform.m_translate = m_matrix.GetPosition();

                CVector rotation = m_matrix.GetRotation();
                m_entity->Placeable.m_transform.m_heading = rotation.fZ;
            }
        }

        void Interpolate(float timeStep) const
        {
            if (m_entity->bDontApplySpeed || m_entity->bDisableMoveForce)
                return;

            CVector interpolation(timeStep, timeStep, timeStep);
            CVector position = m_matrix.GetPosition();
            position += m_linearVelocity * interpolation;

            if (m_entity->Placeable.matrix != nullptr)
            {
                CMatrix matrix = m_matrix;
                matrix.SetPosition(position);

                CVector angular = m_angularVelocity * interpolation;
                AddCrossProduct(matrix.vRight, angular);
                AddCrossProduct(matrix.vFront, angular);
                AddCrossProduct(matrix.vUp, angular);

                m_entity->Placeable.matrix->SetFromMatrix(matrix);
            }
            else
            {
                m_entity->Placeable.m_transform.m_translate = position;
            }
        }

    public:
        CPhysicalSAInterface* m_entity{nullptr};
        CMatrix               m_matrix;
        CVector               m_linearVelocity;
        CVector               m_angularVelocity;
    };

    class SimulationSnapshot
    {
    public:
        bool Contains(CPhysicalSAInterface* entity) const noexcept
        {
            for (const EntitySnapshot& snapshot : m_movingEntities)
            {
                if (snapshot.m_entity == entity)
                    return true;
            }

            return false;
        }

        void Add(CPhysicalSAInterface* entity)
        {
            if (entity != nullptr && m_active)
            {
                m_movingEntities.emplace_back(entity);
            }
        }

        void AddUnique(CPhysicalSAInterface* entity)
        {
            if (entity != nullptr && m_active && !Contains(entity))
            {
                Add(entity);
            }
        }

        void Remove(CPhysicalSAInterface* entity)
        {
            if (!m_active)
                return;

            auto iter = std::remove_if(m_movingEntities.begin(), m_movingEntities.end(), [=](const EntitySnapshot& item) { return item.m_entity == entity; });

            std::for_each(iter, m_movingEntities.end(), [](const EntitySnapshot& item) { item.Restore(); });

            m_movingEntities.erase(iter, m_movingEntities.end());
        }

        void CaptureEntities()
        {
            for (CEntitySAInterface& entity : gtasa::CWorld::ms_listMovingEntityPtrs)
            {
                Add(reinterpret_cast<CPhysicalSAInterface*>(&entity));
            }
        }

        void RestoreEntities()
        {
            for (const EntitySnapshot& snapshot : m_movingEntities)
            {
                snapshot.Restore();
            }

            m_movingEntities.clear();
        }

        void InterpolateEntities(float timeStep) const
        {
            for (const EntitySnapshot& snapshot : m_movingEntities)
            {
                snapshot.Interpolate(timeStep);
            }
        }

        void Suspend() noexcept { m_active = false; }

        void Resume() noexcept { m_active = true; }

        bool IsActive() const noexcept { return m_active; }

    private:
        bool                        m_active{true};
        std::vector<EntitySnapshot> m_movingEntities;
    };

    static SimulationSnapshot g_simulationSnapshot;
}

namespace mtasa
{
    class SimulationLimiter
    {
    public:
        float ProcessTimerUpdate(float deltaCycles)
        {
            m_updateCount = 0;

            // Add clamped delta cycles to our cycles accumulator
            if (deltaCycles > MAX_SIMULATION_TIME_IN_CYCLES)
                deltaCycles = static_cast<float>(MAX_SIMULATION_TIME_IN_CYCLES);

            m_cyclesAccum += deltaCycles;

            // Determine the amount of simulation updates we can run this frame
            while (m_updateCount < 3 && m_cyclesAccum >= SIMULATION_TIME_IN_CYCLES)
            {
                m_cyclesAccum -= SIMULATION_TIME_IN_CYCLES;
                ++m_updateCount;
            }

            // Set some state depending on if we have to do some work this frame
            m_isUpdateFrame = m_updateCount > 0;
            deltaCycles = SIMULATION_TIME_IN_CYCLES * m_updateCount;

            // Update render time step according to game logic from CTimer::UpdateVariables
            m_renderTimeStep = m_cyclesAccum / gtasa::CTimer::ms_cyclesPerMillisecond / 20.0f;

            // Update simulation interpolation rate
            m_simulationInterp = m_cyclesAccum / SIMULATION_TIME_IN_CYCLES;

            if (m_simulationInterp > 1.0f)
                m_simulationInterp = 1.0f;

            return deltaCycles;
        }

        uint32_t GetUpdateCount() const noexcept { return m_updateCount; }

        float GetRenderTimeStep() const noexcept { return m_renderTimeStep; }

        float GetSimulationInterp() const noexcept { return m_simulationInterp; }

        bool IsUpdateFrame() const noexcept { return m_isUpdateFrame; }

    private:
        float    m_cyclesAccum{0};
        uint32_t m_updateCount{0};
        float    m_renderTimeStep{0.0f};
        float    m_simulationInterp{0.0f};
        bool     m_isUpdateFrame{false};
    };

    static SimulationLimiter g_simulationLimiter;
}

using namespace mtasa;

namespace gtasa
{
    void CSkidmarks::UpdateProxy()
    {
        if (g_simulationLimiter.IsUpdateFrame())
        {
            Update();
        }
    }
}

// CWorld
namespace gtasa
{
    void CWorld::ProcessProxy()
    {
        if (g_simulationLimiter.IsUpdateFrame())
        {
            g_simulationSnapshot.RestoreEntities();
            g_simulationSnapshot.Suspend();
            Process();
            g_simulationSnapshot.Resume();
            g_simulationSnapshot.CaptureEntities();
        }
        else
        {
            g_simulationSnapshot.InterpolateEntities(g_simulationLimiter.GetRenderTimeStep());
            ProcessAttachedEntities();
        }
    }
}

// CPhysical
namespace gtasa
{
    void CPhysical::AddToMovingListProxy(CPhysicalSAInterface* self)
    {
        AddToMovingList(self);
        g_simulationSnapshot.AddUnique(self);
    }

    void CPhysical::RemoveFromMovingListProxy(CPhysicalSAInterface* self)
    {
        RemoveFromMovingList(self);
        g_simulationSnapshot.Remove(self);
    }
}

// CTimer
namespace gtasa
{
    void CTimer::InitialiseProxy()
    {
        Initialise();
        SIMULATION_TIME_IN_CYCLES = SIMULATION_TIME_IN_MS * ms_cyclesPerMillisecond;
        MAX_SIMULATION_TIME_IN_CYCLES = MAX_SIMULATION_TIME_IN_MS * ms_cyclesPerMillisecond;
    }

    void CTimer::UpdateVariablesProxy(float deltaCycles)
    {
        float deltaTime = deltaCycles / ms_cyclesPerMillisecond;
        ms_timeInMillisecondsNonClipped += static_cast<uint32_t>(deltaTime);
        ms_timeStepNonClipped = deltaTime / 20.0f;

        if (deltaTime > 300.0f)
            deltaTime = 300.0f;

        ms_timeInMilliseconds += static_cast<uint32_t>(deltaTime);

        if (ms_timeStepNonClipped < 0.01f && !ms_userPause && !ms_codePause && !gtasa::CSpecialFX::ms_isSnapShotActive)
        {
            // ORIG: ms_timeStepNonClipped = 0.01f;
            // MTA:
            ms_timeStepNonClipped = 0.0f;
        }

        ms_previousTimeStep = ms_timeStep;

        if (ms_timeStepNonClipped > 3.0f)
        {
            ms_timeStep = 3.0f;
        }
        else if (ms_timeStepNonClipped > 0.00001f)
        {
            ms_timeStep = ms_timeStepNonClipped;
        }
        else
        {
            // ORIG: ms_timeStep = 0.00001f;
            // MTA:
            ms_timeStep = 0.0f;
        }
    }

    void CTimer::UpdateProxy()
    {
        // MTA: Added for frame limiter
        g_pCore->OnGameTimerUpdate();

        if (!ms_timer)
            return;

        ms_isRunning = true;
        ms_FPS = 1000.0f / (ms_timeInMillisecondsNonClipped - ms_previousTimeInMillisecondsNonClipped);
        ms_PPPPreviousTimeInMilliseconds = ms_PPPreviousTimeInMilliseconds;
        ms_PPPreviousTimeInMilliseconds = ms_PPreviousTimeInMilliseconds;
        ms_PPreviousTimeInMilliseconds = ms_previousTimeInMilliseconds;
        ms_previousTimeInMilliseconds = ms_timeInMilliseconds;
        ms_previousTimeInMillisecondsNonClipped = ms_timeInMillisecondsNonClipped;

        uint64_t now = ms_timer();
        auto     deltaCycles = static_cast<float>(now - ms_lastUpdateTimePoint);

        if (!ms_userPause && !ms_codePause)
            deltaCycles *= ms_timeScale;

        ms_lastUpdateTimePoint = now;

        float deltaTime = deltaCycles / ms_cyclesPerMillisecond;
        ms_timeInMillisecondsPauseMode += static_cast<uint32_t>(deltaTime);

        if (ms_userPause || ms_codePause)
            deltaCycles = 0.0;

        // MTA: Added for simulation limiter
        deltaCycles = g_simulationLimiter.ProcessTimerUpdate(deltaCycles);
        ms_skipProcessThisFrame = !g_simulationLimiter.IsUpdateFrame();

        UpdateVariablesProxy(deltaCycles);

        // MTA: Modified to account for simulation limiter
        // ORIG: ms_frameCounter += 1;
        ms_frameCounter += mtasa::g_simulationLimiter.GetUpdateCount();
    }
}

void CMultiplayerSA::InitHooks_FrameRateFixes()
{
    DetourFunction(gtasa::CTimer::Initialise, gtasa::CTimer::InitialiseProxy);
    DetourFunction(gtasa::CTimer::Update, gtasa::CTimer::UpdateProxy);
    DetourFunction(gtasa::CTimer::UpdateVariables, gtasa::CTimer::UpdateVariablesProxy);
    DetourFunction(gtasa::CSkidmarks::Update, gtasa::CSkidmarks::UpdateProxy);
    DetourFunction(gtasa::CWorld::Process, gtasa::CWorld::ProcessProxy);
    DetourFunction(gtasa::CPhysical::AddToMovingList, gtasa::CPhysical::AddToMovingListProxy);
    DetourFunction(gtasa::CPhysical::RemoveFromMovingList, gtasa::CPhysical::RemoveFromMovingListProxy);
    EZHookInstall(CTaskSimpleUseGun__SetMoveAnim);
}
