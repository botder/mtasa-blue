/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/utils/CHqComms.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

enum
{
    HQCOMMS_STAGE_NONE,
    HQCOMMS_STAGE_TIMER,
    HQCOMMS_STAGE_QUERY,
};

////////////////////////////////////////////////////////////////////
//
// Check with MTA HQ for status updates
//
////////////////////////////////////////////////////////////////////
class CHqComms final
{
public:
    CHqComms(mtasa::IPAddressFamily connectionType) : m_connectionType{connectionType}
    {
        m_strCrashLogFilename = g_pServerInterface->GetAbsolutePath(PathJoin(SERVER_DUMP_PATH, "server_pending_upload.log"));
        m_strCrashDumpMeta = g_pServerInterface->GetAbsolutePath(PathJoin(SERVER_DUMP_PATH, "server_pending_upload_filename"));
    }

    //
    // Send query if it's time
    //
    void Pulse()
    {
        // Check for min client version info
        if (m_Stage == HQCOMMS_STAGE_NONE || (m_Stage == HQCOMMS_STAGE_TIMER && m_CheckTimer.Get() > m_pollInterval))
        {
            m_CheckTimer.Reset();
            m_Stage = HQCOMMS_STAGE_QUERY;

            std::vector<mtasa::IPBindableEndpoint> bindings;

            if (m_connectionType == mtasa::IPAddressFamily::Unspecified)
                bindings = g_pGame->GetConfig()->GetAddressBindings();
            else
                bindings = g_pGame->GetConfig()->GetAddressFamilyBindings(m_connectionType);

            if (bindings.empty())
                return;

            const mtasa::IPAddress& primaryBinding = bindings[0].endpoint.GetAddress();
            std::string             primaryAddress = (primaryBinding.IsUnspecified()) ? "" : primaryBinding.ToString();

            CBitStream bitStream;
            bitStream->Write((char)4);            // Data version
            bitStream->WriteStr(primaryAddress);
            bitStream->Write(g_pGame->GetConfig()->GetServerPort());
            bitStream->WriteStr(CStaticFunctionDefinitions::GetVersionSortable());
            bitStream->Write(g_pGame->GetConfig()->GetMinClientVersionAutoUpdate());
            bitStream->WriteStr(g_pGame->GetConfig()->GetMinClientVersion());
            bitStream->Write(m_iPrevBadFileHashesRev);
            bitStream->Write(g_pGame->GetConfig()->GetHardMaxPlayers());
            bitStream->Write(g_pGame->GetPlayerManager()->Count());
            bitStream->Write(g_pGame->GetConfig()->GetAseInternetPushEnabled() ? 1 : 0);
            bitStream->Write(g_pGame->GetConfig()->GetAseInternetListenEnabled() ? 1 : 0);

            SString strCrashLog;
            FileLoad(m_strCrashLogFilename, strCrashLog, 50000);
            bitStream->WriteStr(strCrashLog);

            // Latest crash dump
            SString strCrashDumpFilename, strCrashDumpContent;
            if (FileExists(m_strCrashDumpMeta))
            {
                if (g_pGame->GetConfig()->GetCrashDumpUploadEnabled())
                {
                    FileLoad(m_strCrashDumpMeta, strCrashDumpFilename);
                    FileLoad(strCrashDumpFilename, strCrashDumpContent);
                }
                // Only attempt to send crashdump once
                FileDelete(m_strCrashDumpMeta);
                m_strCrashDumpMeta = "";
            }
            bitStream->WriteStr(ExtractFilename(strCrashDumpFilename));
            bitStream->WriteStr(strCrashDumpContent);

            bitStream->WriteStr(MTA_OS_STRING);
            bitStream->WriteStr(g_pGame->GetConfig()->GetAddressCommaList(m_connectionType, false));

            bitStream->Write(g_pGame->GetConfig()->IsDatabaseCredentialsProtectionEnabled() ? 1 : 0);
            bitStream->Write(g_pGame->GetConfig()->IsFakeLagCommandEnabled() ? 1 : 0);
            bitStream->Write(g_pGame->GetConfig()->GetAuthSerialHttpEnabled() ? 1 : 0);
            bitStream->WriteStr(SString::Join(",", g_pGame->GetConfig()->GetAuthSerialGroupList()));
            bitStream->WriteStr(SString::Join(",", g_pGame->GetConfig()->GetOwnerEmailAddressList()));

            // Send request
            SHttpRequestOptions options;
            options.strPostData = SStringX((const char*)bitStream->GetData(), bitStream->GetNumberOfBytesUsed());
            options.bPostBinary = true;
            options.uiConnectionAttempts = 2;
            options.connectionType = m_connectionType;
            GetDownloadManager()->QueueFile(m_strURL, NULL, this, StaticDownloadFinishedCallback, options);
        }
    }

    //
    // Process response from hq
    //
    static void StaticDownloadFinishedCallback(const SHttpDownloadResult& result)
    {
        CHqComms* pHqComms = (CHqComms*)result.pObj;
        pHqComms->DownloadFinishedCallback(result);
    }

    //
    // Process response from hq
    //
    void DownloadFinishedCallback(const SHttpDownloadResult& result)
    {
        if (result.bSuccess)
        {
            m_Stage = HQCOMMS_STAGE_TIMER;
            CBitStream bitStream(result.pData, result.dataSize);

            // Process various parts of returned data
            ProcessPollInterval(bitStream);
            ProcessMinClientVersion(bitStream);
            ProcessMessage(bitStream);
            ProcessBadFileHashes(bitStream);
            ProcessCrashInfo(bitStream);
            ProcessAseServers(bitStream);
        }
        else
        {
            m_Stage = HQCOMMS_STAGE_TIMER;
        }
    }

    // Interval until next HQ check
    void ProcessPollInterval(CBitStream& bitStream)
    {
        unsigned int pollInterval = 0;

        if (bitStream->Read(pollInterval) && pollInterval)
            m_pollInterval = std::max<unsigned int>(TICKS_FROM_MINUTES(5), pollInterval);
    }

    // Auto update of min client check
    void ProcessMinClientVersion(CBitStream& bitStream)
    {
        int         iForceSetting = 0;
        CMtaVersion strResultMinClientVersion;

        bitStream->Read(iForceSetting);
        bitStream->ReadStr(strResultMinClientVersion);
        CMtaVersion strSetttingsMinClientVersion = g_pGame->GetConfig()->GetMinClientVersion();
        if (strResultMinClientVersion > strSetttingsMinClientVersion || iForceSetting)
        {
            g_pGame->GetConfig()->SetSetting("minclientversion", strResultMinClientVersion, true);
        }
    }

    // Messsage for this server from HQ
    void ProcessMessage(CBitStream& bitStream)
    {
        int     iMessageAlwaysPrint = 0;
        SString strMessage;

        bitStream->Read(iMessageAlwaysPrint);
        bitStream->ReadStr(strMessage);
        if (!strMessage.empty() && (strMessage != m_strPrevMessage || iMessageAlwaysPrint))
        {
            m_strPrevMessage = strMessage;
            CLogger::LogPrintNoStamp(strMessage);
        }
    }

    // Block script hashes
    void ProcessBadFileHashes(CBitStream& bitStream)
    {
        int  iBadFileHashesRev = 0;
        uint uiNumHashes = 0;
        struct SHashItem
        {
            SString strHash, strReason;
        };
        std::vector<SHashItem> itemList;

        bitStream->Read(iBadFileHashesRev);
        bitStream->Read(uiNumHashes);
        for (uint i = 0; i < uiNumHashes; i++)
        {
            SString strHash, strReason;
            bitStream->ReadStr(strHash);
            if (!bitStream->ReadStr(strReason))
                break;
            itemList.push_back({strHash, strReason});
        }

        if (iBadFileHashesRev && (iBadFileHashesRev == 1 || iBadFileHashesRev != m_iPrevBadFileHashesRev))
        {
            m_iPrevBadFileHashesRev = iBadFileHashesRev;
            g_pGame->GetResourceManager()->ClearBlockedFileReason("");
            for (auto item : itemList)
            {
                g_pGame->GetResourceManager()->AddBlockedFileReason(item.strHash, item.strReason);
            }
            g_pGame->GetResourceManager()->SaveBlockedFileReasons();
        }
    }

    // Got crashinfo recpt
    void ProcessCrashInfo(CBitStream& bitStream)
    {
        int iGotCrashInfo = 0;
        bitStream->Read(iGotCrashInfo);
        if (iGotCrashInfo)
        {
            FileDelete(m_strCrashLogFilename);
        }
    }

    // Extra ASE servers
    void ProcessAseServers(CBitStream& bitStream)
    {
        uint uiNumServers = 0;
        bitStream->Read(uiNumServers);
        for (uint i = 0; i < uiNumServers; i++)
        {
            char    bAcceptsPush, bDoReminders, bHideProblems, bHideSuccess;
            uint    uiReminderIntervalMins;
            SString strDesc, strUrl;
            bitStream->Read(bAcceptsPush);
            bitStream->Read(bDoReminders);
            bitStream->Read(bHideProblems);
            bitStream->Read(bHideSuccess);
            bitStream->Read(uiReminderIntervalMins);
            bitStream->ReadStr(strDesc);
            if (!bitStream->ReadStr(strUrl))
                break;
            g_pGame->GetMasterServerAnnouncer()->AddServer(bAcceptsPush != 0, bDoReminders != 0, bHideProblems != 0, bHideSuccess != 0,
                                                           std::max(5U, uiReminderIntervalMins), strDesc, strUrl);
        }
    }

    //
    // Get http downloader used for hq comms etc.
    //
    static CNetHTTPDownloadManagerInterface* GetDownloadManager() { return g_pNetServer->GetHTTPDownloadManager(EDownloadMode::ASE); }

private:
    

private:
    mtasa::IPAddressFamily m_connectionType;
    unsigned int           m_pollInterval = TICKS_FROM_MINUTES(60);
    int                    m_iPrevBadFileHashesRev = 0;
    uint                   m_Stage = HQCOMMS_STAGE_NONE;
    CElapsedTime           m_CheckTimer;
    SString                m_strURL = HQCOMMS_URL;
    SString                m_strPrevMessage;
    SString                m_strCrashLogFilename;
    SString                m_strCrashDumpMeta;            // Filename of file which contains the latest crash dump filename
};
