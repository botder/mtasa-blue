#pragma once

#include "ResourceFile.h"

class CElementGroup;
class CDummy;
class CEvents;
class CGroups;
class CMarkerManager;
class CBlipManager;
class CObjectManager;
class CPickupManager;
class CPlayerManager;
class CRadarAreaManager;
class CVehicleManager;
class CTeamManager;
class CPedManager;
class CWaterManager;

namespace mtasa
{
    class ResourceMapFile final : public ResourceFile
    {
    public:
        using ResourceFile::ResourceFile;

        void          SetDimension(std::uint16_t dimension) { m_dimension = dimension; }
        std::uint16_t GetDimension() const { return m_dimension; }

    private:
        std::uint16_t  m_dimension = 0;

        CDummy*        m_element = nullptr;
        CElementGroup* m_elementGroup = nullptr;

        CEvents*           m_events;
        CGroups*           m_groups;
        CMarkerManager*    m_markerManager;
        CBlipManager*      m_blipManager;
        CObjectManager*    m_objectManager;
        CPickupManager*    m_pickupManager;
        CPlayerManager*    m_playerManager;
        CRadarAreaManager* m_radarAreaManager;
        CVehicleManager*   m_vehicleManager;
        CTeamManager*      m_teamManager;
        CPedManager*       m_pedManager;
        CWaterManager*     m_waterManager;
    };
}            // namespace mtasa
