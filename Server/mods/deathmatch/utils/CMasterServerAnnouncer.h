/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/utils/CMasterServerAnnouncer.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "ASE.h"

struct SMasterServerDefinition
{
    bool                   bAcceptsPush;
    bool                   bDoReminders;
    bool                   bHideProblems;
    bool                   bHideSuccess;
    uint                   uiReminderIntervalMins;
    SString                strDesc;
    SString                strURL;
    mtasa::IPAddressFamily connectionType;
};

enum
{
    ANNOUNCE_STAGE_NONE,
    ANNOUNCE_STAGE_INITIAL,
    ANNOUNCE_STAGE_INITIAL_RETRY,
    ANNOUNCE_STAGE_REMINDER,
};

////////////////////////////////////////////////////////////////////
//
// A remote master server to announce our existence to
//
////////////////////////////////////////////////////////////////////
class CMasterServer final
{
public:
    CMasterServer(const SMasterServerDefinition& definition) : m_Definition(definition)
    {
#if MTA_DEBUG
        const char* connectionType = "?";

        switch (definition.connectionType)
        {
            case mtasa::IPAddressFamily::IPv4:
                connectionType = "IPv4";
                break;
            case mtasa::IPAddressFamily::IPv6:
                connectionType = "IPv6";
                break;
            default:
                break;
        }

        OutputDebugLine(SString("Added master server (%s): %s", connectionType, definition.strURL.c_str()));
#endif
    }

public:
    //
    // Pulse this master server
    //
    void Pulse()
    {
        if (m_bStatusBusy)
            return;

        long long llTickCountNow = GetTickCount64_();

        // Do announce?
        if (g_pGame->GetConfig()->GetAseInternetListenEnabled())
        {
            bool bIsTimeForAnnounce = false;
            if (m_Stage == ANNOUNCE_STAGE_INITIAL)
                bIsTimeForAnnounce = true;
            if (m_Stage == ANNOUNCE_STAGE_INITIAL_RETRY && m_Definition.bDoReminders &&
                llTickCountNow - m_llLastAnnounceTime > m_uiInitialAnnounceRetryInterval)
                bIsTimeForAnnounce = true;
            if (m_Stage == ANNOUNCE_STAGE_REMINDER && m_Definition.bDoReminders &&
                llTickCountNow - m_llLastAnnounceTime > TICKS_FROM_MINUTES(m_Definition.uiReminderIntervalMins))
                bIsTimeForAnnounce = true;

            if (bIsTimeForAnnounce)
            {
                m_llLastAnnounceTime = llTickCountNow;

                // Send request
                m_bStatusBusy = true;
                SHttpRequestOptions options;
                options.uiConnectionAttempts = 2;
                options.connectionType = m_Definition.connectionType;
                GetDownloadManager()->QueueFile(m_Definition.strURL, NULL, this, StaticDownloadFinishedCallback, options);
            }
        }
        else
        {
            m_Stage = ANNOUNCE_STAGE_INITIAL;
        }

        // Do push?
        if (g_pGame->GetConfig()->GetAseInternetPushEnabled() && m_Definition.bAcceptsPush)
        {
            if (m_llLastPushTime == 0 || llTickCountNow - m_llLastPushTime > m_uiPushInterval)
            {
                m_llLastPushTime = llTickCountNow;
                SHttpRequestOptions options;
                options.strPostData = ASE::GetInstance()->QueryLight();
                options.bPostBinary = true;
                options.uiConnectionAttempts = 1;
                options.connectionType = m_Definition.connectionType;
                GetDownloadManager()->QueueFile(m_Definition.strURL, NULL, NULL, NULL, options);
            }
        }
    }

    //
    // Process response from master server
    //
    static void StaticDownloadFinishedCallback(const SHttpDownloadResult& result)
    {
        CMasterServer* pMasterServer = (CMasterServer*)result.pObj;
        pMasterServer->DownloadFinishedCallback(result);
    }

    //
    // Process response from master server
    //
    void DownloadFinishedCallback(const SHttpDownloadResult& result)
    {
        m_bStatusBusy = false;

        if (result.bSuccess)
        {
            if (m_Stage < ANNOUNCE_STAGE_REMINDER)
            {
                m_Stage = ANNOUNCE_STAGE_REMINDER;
                if (!m_Definition.bHideSuccess)
                {
                    CArgMap argMap;
                    argMap.SetFromString(result.pData);
                    SString strOkMessage = argMap.Get("ok_message");

                    // Log successful initial announcement
                    if (result.iErrorCode == 200)
                        CLogger::LogPrintf("%s success! %s\n", *m_Definition.strDesc, *strOkMessage);
                    else
                        CLogger::LogPrintf("%s successish! (%u %s)\n", *m_Definition.strDesc, result.iErrorCode, GetDownloadManager()->GetError());
                }
            }
        }
        else
        {
            bool bCanRetry = (result.iErrorCode == 28);            // We can retry if 'Timeout was reached'

            if (m_Stage == ANNOUNCE_STAGE_INITIAL)
            {
                if (bCanRetry)
                {
                    m_Stage = ANNOUNCE_STAGE_INITIAL_RETRY;
                    if (!m_Definition.bHideProblems)
                    {
                        // Log initial failure
                        CLogger::LogPrintf("%s no response. Retrying...\n", *m_Definition.strDesc);
                    }
                }
            }

            if (m_Stage < ANNOUNCE_STAGE_REMINDER)
            {
                if (!bCanRetry || m_uiInitialAnnounceRetryAttempts-- == 0)
                {
                    // Give up initial retries
                    m_Stage = ANNOUNCE_STAGE_REMINDER;
                    if (!m_Definition.bHideProblems)
                    {
                        // Log final failure
                        CLogger::LogPrintf("%s failed! (%u %s)\n", *m_Definition.strDesc, result.iErrorCode, GetDownloadManager()->GetError());
                    }
                }
            }
        }
    }

    const SMasterServerDefinition& GetDefinition() const { return m_Definition; }

    //
    // Get http downloader used for master server comms etc.
    //
    static CNetHTTPDownloadManagerInterface* GetDownloadManager() { return g_pNetServer->GetHTTPDownloadManager(EDownloadMode::ASE); }

protected:
    bool                          m_bStatusBusy = false;
    uint                          m_Stage = ANNOUNCE_STAGE_INITIAL;
    uint                          m_uiInitialAnnounceRetryAttempts = 5;
    uint                          m_uiInitialAnnounceRetryInterval = 5 * 60000;            // 5 mins initial announce retry interval
    uint                          m_uiPushInterval = 10 * 60000;                           // 10 mins push interval
    long long                     m_llLastAnnounceTime = 0;
    long long                     m_llLastPushTime = 0;
    const SMasterServerDefinition m_Definition;
};

////////////////////////////////////////////////////////////////////
//
// A list of remote master servers to announce our existence to
//
////////////////////////////////////////////////////////////////////
class CMasterServerAnnouncer final
{
    using Replacement = std::pair<std::string, std::string>;
    using PrimaryAddress = std::pair<mtasa::IPAddressFamily, std::string>;

public:
    CMasterServerAnnouncer()
    {
        CMainConfig*  config = g_pGame->GetConfig();
        bool          hasPassword = config->HasPassword();
        bool          aseLanListen = config->GetAseLanListenEnabled();
        std::uint16_t gamePort = config->GetServerPort();
        unsigned int  maxPlayers = config->GetMaxPlayers();
        SString       aseMode = config->GetSetting("ase");

        SString version("%d.%d.%d-%d.%05d", MTASA_VERSION_MAJOR, MTASA_VERSION_MINOR, MTASA_VERSION_MAINTENANCE, MTASA_VERSION_TYPE, MTASA_VERSION_BUILD);
        SString extra("%d_%d_%d_%s_%d", 0, maxPlayers, hasPassword, *aseMode, aseLanListen);
        
        m_replacements = {
            Replacement{"%GAME%", std::to_string(gamePort)},
            Replacement{"%ASE%", std::to_string(gamePort + SERVER_LIST_QUERY_PORT_OFFSET)},
            Replacement{"%HTTP%", std::to_string(config->GetHTTPPort())},
            Replacement{"%VER%", version},
            Replacement{"%EXTRA%", extra},
        };

        bool hasPrimaryIPv4 = false;
        bool hasPrimaryIPv6 = false;

        for (const mtasa::IPBindableEndpoint& binding : config->GetAddressBindings())
        {
            std::string ip = (binding.endpoint.GetAddress().IsUnspecified()) ? "" : binding.endpoint.GetAddress().ToString();

            switch (binding.endpoint.GetAddressFamily())
            {
                case mtasa::IPAddressFamily::IPv4:
                    if (!hasPrimaryIPv4)
                    {
                        hasPrimaryIPv4 = true;
                        m_primaryAddresses.emplace_back(mtasa::IPAddressFamily::IPv4, ip);
                    }
                    break;
                case mtasa::IPAddressFamily::IPv6:
                    if (!hasPrimaryIPv6)
                    {
                        hasPrimaryIPv6 = true;
                        m_primaryAddresses.emplace_back(mtasa::IPAddressFamily::IPv6, ip);

                        if (binding.useDualMode)
                        {
                            hasPrimaryIPv4 = true;
                            m_primaryAddresses.emplace_back(mtasa::IPAddressFamily::IPv4, ip);
                        }
                    }
                    break;
                case mtasa::IPAddressFamily::Unspecified:
                    break;
            }

            if (hasPrimaryIPv4 || hasPrimaryIPv6)
                break;
        }

        AddServer(true, true, false, false, 60 * 24, "Querying MTA master server...", QUERY_URL_MTA_MASTER_SERVER);
    }

    void AddServer(bool bAcceptsPush, bool bDoReminders, bool bHideProblems, bool bHideSuccess, uint uiReminderIntervalMins, const SString& strDesc,
                   const SString& strInUrl)
    {
        // Check if server is already present
        for (std::unique_ptr<CMasterServer>& masterServer : m_masterServers)
        {
            if (masterServer->GetDefinition().strURL.BeginsWithI(strInUrl.SplitLeft("%")))
                return;
        }

        SString basicUrl = strInUrl;

        for (const Replacement& pair : m_replacements)
            basicUrl = basicUrl.Replace(pair.first.c_str(), pair.second.c_str());

        for (const PrimaryAddress& primaryAddress : m_primaryAddresses)
        {
            SString                 masterServerUrl = basicUrl.Replace("%IP%", primaryAddress.second.c_str());
            SMasterServerDefinition masterServerDefinition = {bAcceptsPush,           bDoReminders, bHideProblems,   bHideSuccess,
                                                              uiReminderIntervalMins, strDesc,      masterServerUrl, primaryAddress.first};
            m_masterServers.push_back(std::make_unique<CMasterServer>(masterServerDefinition));
        }
    }

    //
    // Pulse each master server in our list
    //
    void Pulse()
    {
        for (std::unique_ptr<CMasterServer>& masterServer : m_masterServers)
            masterServer->Pulse();
    }

private:
    std::vector<std::unique_ptr<CMasterServer>> m_masterServers;
    std::vector<Replacement>                    m_replacements;
    std::vector<PrimaryAddress>                 m_primaryAddresses;
};
