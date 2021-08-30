/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CHTTPD.cpp
 *  PURPOSE:     Built-in HTTP webserver class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CHTTPD.h"
#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>
#include <SharedUtil.Crypto.h>
#include <mtasa/IPAddress.h>
#include <ehs/ehs.h>

using namespace mtasa;

extern CGame* g_pGame;

class CHTTPD::Impl final : public EHS
{
public:
    Impl()
    {
        m_guestAccount = g_pGame->GetAccountManager()->AddGuestAccount(HTTP_GUEST_ACCOUNT_NAME);

        // Max of 'n' connections per 10 seconds, then 1 minute ignore
        m_httpDosProtect = CConnectHistory(g_pGame->GetConfig()->GetHTTPDosThreshold(), 10000, 60000 * 1);

        std::vector<SString> excludeList;
        g_pGame->GetConfig()->GetHTTPDosExclude().Split(",", excludeList);
        m_httpDosExcludeMap = std::set<SString>(excludeList.begin(), excludeList.end());
    }

public:
    CAccount* CheckAuthentication(HttpRequest* request)
    {
        string authorization = request->oRequestHeaders["authorization"];
        if (authorization.length() > 6)
        {
            if (authorization.substr(0, 6) == "Basic ")
            {
                // Basic auth
                SString strAuthDecoded = SharedUtil::Base64decode(authorization.substr(6));

                SString authName, authPassword;
                strAuthDecoded.Split(":", &authName, &authPassword);

                if (m_bruteForceProtect.IsFlooding(request->GetAddress().c_str()))
                {
                    CLogger::AuthPrintf("HTTP: Ignoring login attempt for user '%s' from %s\n", authName.c_str(), request->GetAddress().c_str());
                    return m_guestAccount;
                }

                CAccount* account = g_pGame->GetAccountManager()->Get(authName.c_str());
                if (account)
                {
                    // Check that the password is right
                    bool bSkipIpCheck;
                    if (account->IsPassword(authPassword.c_str(), &bSkipIpCheck))
                    {
                        // Check that it isn't the Console account
                        std::string strAccountName = account->GetName();
                        if (strAccountName.compare(CONSOLE_ACCOUNT_NAME) != 0)
                        {
                            // Do IP check if required
                            if (!bSkipIpCheck && !g_pGame->GetAccountManager()->IsHttpLoginAllowed(account, request->GetAddress()))
                            {
                                if (m_warnMessageTimer.Get() > 8000 || m_warnMessageForAddress != request->GetAddress())
                                {
                                    m_warnMessageForAddress = request->GetAddress();
                                    m_warnMessageTimer.Reset();
                                }
                                CLogger::AuthPrintf("HTTP: Failed login for user '%s' because %s not associated with authorized serial\n", authName.c_str(),
                                                    request->GetAddress().c_str());
                                return m_guestAccount;
                            }

                            // Handle initial login logging
                            std::lock_guard<std::mutex> guard(m_mutexLoggedInMap);
                            if (m_LoggedInMap.find(authName) == m_LoggedInMap.end())
                                CLogger::AuthPrintf("HTTP: '%s' entered correct password from %s\n", authName.c_str(), request->GetAddress().c_str());
                            m_LoggedInMap[authName] = GetTickCount64_();
                            account->OnLoginHttpSuccess(request->GetAddress());
                            return account;
                        }
                    }
                }
                if (authName.length() > 0)
                {
                    m_bruteForceProtect.AddConnect(request->GetAddress().c_str());
                    CLogger::AuthPrintf("HTTP: Failed login attempt for user '%s' from %s\n", authName.c_str(), request->GetAddress().c_str());
                }
            }
        }

        return m_guestAccount;
    }

    HttpResponse* RouteRequest(HttpRequest* request) override
    {
        if (!g_pGame->IsServerFullyUp())
        {
            // create an HttpRespose object for the message
            HttpResponse* response = new HttpResponse(request->m_nRequestId, request->m_poSourceEHSConnection);
            response->SetBody("The server is not ready. Please try again in a minute.", 54);
            response->m_nResponseCode = HTTPRESPONSECODE_200_OK;
            return response;
        }

        // Sync with main thread before routing (to a resource)
        g_pGame->Lock();
        HttpResponse* response = EHS::RouteRequest(request);
        g_pGame->Unlock();

        return response;
    }

    ResponseCode HandleRequest(HttpRequest* request, HttpResponse* response) override
    {
        // Check if server verification was requested
        auto challenge = request->oRequestHeaders["crypto_challenge"];

        if (request->sUri == "/get_verification_key_code" && challenge != "")
        {
            auto    path = g_pServerInterface->GetModManager()->GetAbsolutePath("verify.key");
            SString encodedPublicKey;
            SharedUtil::FileLoad(path, encodedPublicKey, 392);

            using namespace CryptoPP;

            try
            {
                // Load public RSA key from disk
                RSA::PublicKey publicKey;
                std::string    base64Data = SharedUtil::Base64decode(encodedPublicKey);
                StringSource   stringSource(base64Data, true);
                publicKey.Load(stringSource);

                // Launch encryptor and encrypt
                RSAES_OAEP_SHA_Encryptor encryptor(publicKey);
                SecByteBlock             cipherText(encryptor.CiphertextLength(challenge.size()));
                AutoSeededRandomPool     rng;
                encryptor.Encrypt(rng, (const CryptoPP::byte*)challenge.data(), challenge.size(), cipherText.begin());

                if (!cipherText.empty())
                {
                    response->SetBody((const char*)cipherText.BytePtr(), cipherText.SizeInBytes());
                    return HTTPRESPONSECODE_200_OK;
                }
                else
                    CLogger::LogPrintf(LOGLEVEL_MEDIUM, "ERROR: Empty crypto challenge was passed during verification\n");
            }
            catch (const std::exception&)
            {
                CLogger::LogPrintf(LOGLEVEL_MEDIUM, "ERROR: Invalid verify.key keyfile\n");
            }

            response->SetBody("", 0);
            return HTTPRESPONSECODE_401_UNAUTHORIZED;
        }

        CAccount* account = CheckAuthentication(request);

        if (account)
        {
            if (!m_defaultResourceName.empty())
            {
                SString strWelcome("<a href='/%s/'>This is the page you want</a>", m_defaultResourceName.c_str());
                response->SetBody(strWelcome.c_str(), strWelcome.size());
                SString strNewURL("http://%s/%s/", request->oRequestHeaders["host"].c_str(), m_defaultResourceName.c_str());
                response->oResponseHeaders["location"] = strNewURL.c_str();
                return HTTPRESPONSECODE_302_FOUND;
            }
        }

        SString strWelcome(
            "You haven't set a default resource in your configuration file. You can either do this or visit http://%s/<i>resourcename</i>/ to see a specific "
            "resource.<br/><br/>Alternatively, the server may be still starting up, if so, please try again in a minute.",
            request->oRequestHeaders["host"].c_str());
        response->SetBody(strWelcome.c_str(), strWelcome.size());
        return HTTPRESPONSECODE_200_OK;
    }

    void HttpPulse() override
    {
        std::lock_guard<std::mutex> guard(m_mutexLoggedInMap);

        long long llExpireTime = GetTickCount64_() - 1000 * 60 * 5;            // 5 minute timeout

        map<string, long long>::iterator iter = m_LoggedInMap.begin();
        while (iter != m_LoggedInMap.end())
        {
            // Remove if too long since last request
            if (iter->second < llExpireTime)
            {
                CLogger::AuthPrintf("HTTP: '%s' no longer connected\n", iter->first.c_str());
                m_LoggedInMap.erase(iter++);
            }
            else
                iter++;
        }
    }

    bool ShouldAllowConnection(const char* address) override
    {
        std::lock_guard<std::mutex> guard(m_mutexHttpDosProtect);

        if (MapContains(m_httpDosExcludeMap, address))
            return true;

        if (m_httpDosProtect.IsFlooding(address))
            return false;

        m_httpDosProtect.AddConnect(address);

        if (m_httpDosProtect.IsFlooding(address))
        {
            CLogger::AuthPrintf("HTTP: Connection flood from '%s'. Ignoring for 1 min.\n", address);
            return false;
        }

        return true;
    }

    int RequestLogin(HttpRequest* request, HttpResponse* response)
    {
        if (m_warnMessageTimer.Get() < 4000 && m_warnMessageForAddress == request->GetAddress())
        {
            SString strMessage;
            strMessage += SString("Your IP address ('%s') is not associated with an authorized serial.", request->GetAddress().c_str());
            strMessage += SString("<br/><a href='%s'>See here for more information</a>",
                                  "https://mtasa.com/authserialhttp");
            response->SetBody(strMessage, strMessage.length());
            return HTTPRESPONSECODE_401_UNAUTHORIZED;
        }

        const char* szAuthenticateMessage = "Access denied, please login";
        response->SetBody(szAuthenticateMessage, strlen(szAuthenticateMessage));
        SString strName("Basic realm=\"%s\"", g_pGame->GetConfig()->GetServerName().c_str());
        response->oResponseHeaders["WWW-Authenticate"] = strName.c_str();
        return HTTPRESPONSECODE_401_UNAUTHORIZED;
    }

    void SetDefaultResourceName(std::string_view resourceName) { m_defaultResourceName = resourceName; }

private:
    CAccount*                   m_guestAccount;
    std::string                 m_defaultResourceName;
    CConnectHistory             m_httpDosProtect{0, 0, 0};
    CConnectHistory             m_bruteForceProtect{4, 30000, 60000 * 5};            // Max of 4 attempts per 30 seconds, then 5 minute ignore
    std::set<SString>           m_httpDosExcludeMap;
    std::mutex                  m_mutexHttpDosProtect;
    std::mutex                  m_mutexLoggedInMap;
    std::map<string, long long> m_LoggedInMap;
    CElapsedTime                m_warnMessageTimer;
    SString                     m_warnMessageForAddress;
};

CHTTPD::CHTTPD() : m_impl(std::make_unique<Impl>())
{
}

CHTTPD::~CHTTPD() = default;

bool CHTTPD::Start(const std::vector<IPBindableEndpoint>& bindings)
{
    EHSServerParameters parameters;
    parameters["mode"] = "threadpool";
    parameters["threadcount"] = g_pGame->GetConfig()->GetHTTPThreadCount();
    parameters["bindings"] = bindings;
    m_isRunning = (m_impl->StartServer(parameters) == EHS::STARTSERVER_SUCCESS);
    return m_isRunning;
}

void CHTTPD::Stop()
{
    if (m_isRunning)
    {
        m_impl->StopServer();
        m_isRunning = false;
    }
}

int CHTTPD::RequestLogin(HttpRequest* request, HttpResponse* response)
{
    return m_impl->RequestLogin(request, response);
}

CAccount* CHTTPD::CheckAuthentication(HttpRequest* request)
{
    return m_impl->CheckAuthentication(request);
}

void CHTTPD::SetDefaultResourceName(std::string_view resourceName)
{
    m_impl->SetDefaultResourceName(resourceName);
}

int CHTTPD::RegisterEHS(EHS* ehs, const char* path)
{
    return static_cast<int>(m_impl->RegisterEHS(ehs, path));
}

int CHTTPD::UnregisterEHS(const char* path)
{
    return static_cast<int>(m_impl->UnregisterEHS(path));
}
