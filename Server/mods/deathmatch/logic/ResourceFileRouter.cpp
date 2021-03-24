/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Resource file router for embedded HTTP web server
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "ResourceFileRouter.h"
#include "CResource.h"
#include "CResourceHTMLItem.h"

namespace mtasa
{
    ResourceFileRouter::ResourceFileRouter(CResource& resource) : m_resource{resource}
    {
        g_pGame->GetHTTPD()->RegisterEHS(this, resource.m_strResourceName.c_str());
        m_oEHSServerParameters["norouterequest"] = true;
        RegisterEHS(this, "call");
    }

    ResourceFileRouter::~ResourceFileRouter()
    {
        UnregisterEHS("call");
        g_pGame->GetHTTPD()->UnregisterEHS(m_resource.m_strResourceName.c_str());
    }

    ResponseCode ResourceFileRouter::HandleRequest(HttpRequest* ipoHttpRequest, HttpResponse* ipoHttpResponse)
    {
        std::string strAccessType;
        const char* szRequest = ipoHttpRequest->sOriginalUri.c_str();

        if (*szRequest)
        {
            const char* szSlash1 = strchr(szRequest + 1, '/');

            if (szSlash1)
            {
                const char* szSlash2 = strchr(szSlash1 + 1, '/');

                if (szSlash2)
                    strAccessType.assign(szSlash1 + 1, szSlash2 - (szSlash1 + 1));
            }
        }

        CAccount* account = g_pGame->GetHTTPD()->CheckAuthentication(ipoHttpRequest);

        if (account != nullptr)
        {
            ResponseCode responseCode;

            if (strAccessType == "call")
                responseCode = HandleCallRequest(ipoHttpRequest, ipoHttpResponse, account);
            else
                responseCode = HandleFileRequest(ipoHttpRequest, ipoHttpResponse, account);

            return responseCode;
        }

        return HTTPRESPONSECODE_200_OK;
    }

    ResponseCode ResourceFileRouter::HandleFileRequest(HttpRequest* ipoHttpRequest, HttpResponse* ipoHttpResponse, CAccount* account)
    {
        const char* szUrl = ipoHttpRequest->sOriginalUri.c_str();
        std::string strFile;

        if (szUrl[0])
        {
            const char* pFileFrom = strchr(szUrl[0] == '/' ? &szUrl[1] : szUrl, '/');

            if (pFileFrom)
            {
                pFileFrom++;
                const char* pFileTo = strchr(pFileFrom, '?');

                if (pFileTo)
                    strFile.assign(pFileFrom, pFileTo - pFileFrom);
                else
                    strFile = pFileFrom;
            }
        }

        // Unescape(strFile);

        for (CResourceFile* pResourceFile : m_resource.m_ResourceFiles)
        {
            if (!strFile.empty())
            {
                if (strcmp(pResourceFile->GetName(), strFile.c_str()) != 0)
                    continue;

                if (pResourceFile->GetType() == CResourceFile::RESOURCE_FILE_TYPE_HTML)
                {
                    CResourceHTMLItem* pHtml = (CResourceHTMLItem*)pResourceFile;

                    // We need to be active if downloading a HTML file
                    if (m_resource.m_eState == EResourceState::Running)
                    {
                        if (!IsHttpAccessAllowed(account))
                        {
                            return g_pGame->GetHTTPD()->RequestLogin(ipoHttpRequest, ipoHttpResponse);
                        }

                        SString strResourceFileName("%s.file.%s", m_resource.m_strResourceName.c_str(), pHtml->GetName());
                        if (g_pGame->GetACLManager()->CanObjectUseRight(account->GetName().c_str(), CAccessControlListGroupObject::OBJECT_TYPE_USER,
                                                                        strResourceFileName.c_str(), CAccessControlListRight::RIGHT_TYPE_RESOURCE,
                                                                        !pHtml->IsRestricted()))
                        {
                            return pHtml->Request(ipoHttpRequest, ipoHttpResponse, account);
                        }

                        return g_pGame->GetHTTPD()->RequestLogin(ipoHttpRequest, ipoHttpResponse);
                    }
                    else
                    {
                        ipoHttpResponse->SetBody("error: resource not running", 28);
                        return HTTPRESPONSECODE_401_UNAUTHORIZED;
                    }
                }
                // Send back any clientfile. Otherwise keep looking for server files matching
                // this filename. If none match, the file not found will be sent back.
                else if (pResourceFile->GetType() == CResourceFile::RESOURCE_FILE_TYPE_CLIENT_CONFIG ||
                         pResourceFile->GetType() == CResourceFile::RESOURCE_FILE_TYPE_CLIENT_SCRIPT ||
                         pResourceFile->GetType() == CResourceFile::RESOURCE_FILE_TYPE_CLIENT_FILE)
                {
                    return pResourceFile->Request(ipoHttpRequest, ipoHttpResponse);            // sends back any file in the resource
                }
            }
            else            // handle the default page
            {
                if (!IsHttpAccessAllowed(account))
                {
                    return g_pGame->GetHTTPD()->RequestLogin(ipoHttpRequest, ipoHttpResponse);
                }

                if (pResourceFile->GetType() == CResourceFile::RESOURCE_FILE_TYPE_HTML)
                {
                    CResourceHTMLItem* pHtml = (CResourceHTMLItem*)pResourceFile;

                    if (pHtml->IsDefaultPage())
                    {
                        CAccessControlListManager* pACLManager = g_pGame->GetACLManager();

                        SString strResourceFileName("%s.file.%s", m_resource.m_strResourceName.c_str(), pResourceFile->GetName());
                        if (pACLManager->CanObjectUseRight(account->GetName().c_str(), CAccessControlListGroupObject::OBJECT_TYPE_USER,
                                                           m_resource.m_strResourceName.c_str(), CAccessControlListRight::RIGHT_TYPE_RESOURCE, true) &&
                            pACLManager->CanObjectUseRight(account->GetName().c_str(), CAccessControlListGroupObject::OBJECT_TYPE_USER,
                                                           strResourceFileName.c_str(), CAccessControlListRight::RIGHT_TYPE_RESOURCE, !pHtml->IsRestricted()))
                        {
                            return pHtml->Request(ipoHttpRequest, ipoHttpResponse, account);
                        }
                        else
                        {
                            return g_pGame->GetHTTPD()->RequestLogin(ipoHttpRequest, ipoHttpResponse);
                        }
                    }
                }
            }
        }

        ipoHttpResponse->SetBody("error: resource file not found", 31);
        return HTTPRESPONSECODE_404_NOTFOUND;
    }

    ResponseCode ResourceFileRouter::HandleCallRequest(HttpRequest* ipoHttpRequest, HttpResponse* ipoHttpResponse, CAccount* account)
    {
        if (!IsHttpAccessAllowed(account))
        {
            return g_pGame->GetHTTPD()->RequestLogin(ipoHttpRequest, ipoHttpResponse);
        }

#define MAX_INPUT_VARIABLES 25

        if (m_resource.m_eState != EResourceState::Running)
        {
            ipoHttpResponse->SetBody("error: resource not running", 28);
            return HTTPRESPONSECODE_200_OK;
        }

        const char* szQueryString = ipoHttpRequest->sUri.c_str();

        if (!*szQueryString)
        {
            ipoHttpResponse->SetBody("error: invalid function name", 29);
            return HTTPRESPONSECODE_200_OK;
        }

        std::string              strFuncName;
        std::vector<std::string> vecArguments;
        const char*              pQueryArg = strchr(szQueryString, '?');

        if (!pQueryArg)
        {
            strFuncName = szQueryString;
        }
        else
        {
            strFuncName.assign(szQueryString, pQueryArg - szQueryString);
            pQueryArg++;

            const char* pEqual = nullptr;
            const char* pAnd = nullptr;

            while (*pQueryArg)
            {
                pAnd = strchr(pQueryArg, '&');

                if (!pAnd)
                    pAnd = pQueryArg + strlen(pQueryArg);

                pEqual = strchr(pQueryArg, '=');

                if (pEqual && pEqual < pAnd)
                {
                    std::string strKey(pQueryArg, pEqual - pQueryArg);
                    int         iKey = atoi(strKey.c_str());

                    if (iKey >= 0 && iKey < MAX_INPUT_VARIABLES)
                    {
                        std::string strValue(pEqual + 1, pAnd - (pEqual + 1));
                        // Unescape(strValue);

                        if (iKey + 1 > static_cast<int>(vecArguments.size()))
                            vecArguments.resize(iKey + 1);

                        vecArguments[iKey] = strValue;
                    }
                }

                if (*pAnd)
                    pQueryArg = pAnd + 1;
                else
                    break;
            }
        }

        // Unescape(strFuncName);

        for (CExportedFunction& Exported : m_resource.m_ExportedFunctions)
        {
            if (strFuncName != Exported.GetFunctionName())
                continue;

            if (!Exported.IsHTTPAccessible())
                return g_pGame->GetHTTPD()->RequestLogin(ipoHttpRequest, ipoHttpResponse);

            SString strResourceFuncName("%s.function.%s", m_resource.m_strResourceName.c_str(), strFuncName.c_str());

            // @@@@@ Deal with this the new way
            if (!g_pGame->GetACLManager()->CanObjectUseRight(account->GetName().c_str(), CAccessControlListGroupObject::OBJECT_TYPE_USER,
                                                             strResourceFuncName.c_str(), CAccessControlListRight::RIGHT_TYPE_RESOURCE, true))
            {
                return g_pGame->GetHTTPD()->RequestLogin(ipoHttpRequest, ipoHttpResponse);
            }

            CLuaArguments Arguments;

            if (ipoHttpRequest->nRequestMethod == REQUESTMETHOD_GET)
            {
                for (const std::string& strArgument : vecArguments)
                {
                    const char* szArg = strArgument.c_str();

                    if (strlen(szArg) > 3 && szArg[0] == '^' && szArg[2] == '^' && szArg[1] != '^')
                    {
                        switch (szArg[1])
                        {
                            case 'E':            // element
                            {
                                int       id = atoi(szArg + 3);
                                CElement* pElement = nullptr;

                                if (id != INT_MAX && id != INT_MIN && id != 0)
                                    pElement = CElementIDs::GetElement(id);

                                if (pElement)
                                {
                                    Arguments.PushElement(pElement);
                                }
                                else
                                {
                                    g_pGame->GetScriptDebugging()->LogError(nullptr, "HTTP Get - Invalid element specified.");
                                    Arguments.PushNil();
                                }

                                break;
                            }
                            case 'R':            // resource
                            {
                                CResource* pResource = g_pGame->GetResourceManager()->GetResource(szArg + 3);

                                if (pResource)
                                {
                                    Arguments.PushResource(pResource);
                                }
                                else
                                {
                                    g_pGame->GetScriptDebugging()->LogError(nullptr, "HTTP Get - Invalid resource specified.");
                                    Arguments.PushNil();
                                }

                                break;
                            }
                        }
                    }
                    else
                    {
                        Arguments.PushString(szArg);
                    }
                }
            }
            else if (ipoHttpRequest->nRequestMethod == REQUESTMETHOD_POST)
            {
                const char* szRequestBody = ipoHttpRequest->sBody.c_str();
                Arguments.ReadFromJSONString(szRequestBody);
            }

            CLuaArguments FormData;
            for (const auto& pair : ipoHttpRequest->oFormValueMap)
            {
                FormData.PushString(pair.first.c_str());
                FormData.PushString(pair.second.sBody.c_str());
            }

            CLuaArguments Cookies;
            for (const auto& pair : ipoHttpRequest->oCookieMap)
            {
                Cookies.PushString(pair.first.c_str());
                Cookies.PushString(pair.second.c_str());
            }

            CLuaArguments Headers;
            for (const auto& pair : ipoHttpRequest->oRequestHeaders)
            {
                Headers.PushString(pair.first.c_str());
                Headers.PushString(pair.second.c_str());
            }

            // TODO: Remove later
            CLuaMain* m_pVM = m_resource.m_pVM;

            LUA_CHECKSTACK(m_pVM->GetVM(), 1);            // Ensure some room

            // cache old data
            lua_getglobal(m_pVM->GetVM(), "form");
            CLuaArgument OldForm(m_pVM->GetVM(), -1);
            lua_pop(m_pVM->GetVM(), 1);

            lua_getglobal(m_pVM->GetVM(), "cookies");
            CLuaArgument OldCookies(m_pVM->GetVM(), -1);
            lua_pop(m_pVM->GetVM(), 1);

            lua_getglobal(m_pVM->GetVM(), "requestHeaders");
            CLuaArgument OldHeaders(m_pVM->GetVM(), -1);
            lua_pop(m_pVM->GetVM(), 1);

            lua_getglobal(m_pVM->GetVM(), "hostname");
            CLuaArgument OldHostname(m_pVM->GetVM(), -1);
            lua_pop(m_pVM->GetVM(), 1);

            lua_getglobal(m_pVM->GetVM(), "url");
            CLuaArgument OldURL(m_pVM->GetVM(), -1);
            lua_pop(m_pVM->GetVM(), 1);

            lua_getglobal(m_pVM->GetVM(), "user");
            CLuaArgument OldUser(m_pVM->GetVM(), -1);
            lua_pop(m_pVM->GetVM(), 1);

            // push new data
            FormData.PushAsTable(m_pVM->GetVM());
            lua_setglobal(m_pVM->GetVM(), "form");

            Cookies.PushAsTable(m_pVM->GetVM());
            lua_setglobal(m_pVM->GetVM(), "cookies");

            Headers.PushAsTable(m_pVM->GetVM());
            lua_setglobal(m_pVM->GetVM(), "requestHeaders");

            lua_pushstring(m_pVM->GetVM(), ipoHttpRequest->GetAddress().c_str());
            lua_setglobal(m_pVM->GetVM(), "hostname");

            lua_pushstring(m_pVM->GetVM(), ipoHttpRequest->sOriginalUri.c_str());
            lua_setglobal(m_pVM->GetVM(), "url");

            lua_pushaccount(m_pVM->GetVM(), account);
            lua_setglobal(m_pVM->GetVM(), "user");

            CLuaArguments Returns;
            Arguments.CallGlobal(m_pVM, strFuncName.c_str(), &Returns);
            // g_pGame->Unlock(); // release the mutex

            // restore old data
            OldForm.Push(m_pVM->GetVM());
            lua_setglobal(m_pVM->GetVM(), "form");

            OldCookies.Push(m_pVM->GetVM());
            lua_setglobal(m_pVM->GetVM(), "cookies");

            OldHeaders.Push(m_pVM->GetVM());
            lua_setglobal(m_pVM->GetVM(), "requestHeaders");

            OldHostname.Push(m_pVM->GetVM());
            lua_setglobal(m_pVM->GetVM(), "hostname");

            OldURL.Push(m_pVM->GetVM());
            lua_setglobal(m_pVM->GetVM(), "url");

            OldUser.Push(m_pVM->GetVM());
            lua_setglobal(m_pVM->GetVM(), "user");

            // Set debug info in case error occurs in WriteToJSONString
            g_pGame->GetScriptDebugging()->SaveLuaDebugInfo(
                SLuaDebugInfo(m_resource.m_strResourceName, INVALID_LINE_NUMBER, SString("[HTTP:%s]", strFuncName.c_str())));

            std::string strJSON;
            Returns.WriteToJSONString(strJSON, true);

            g_pGame->GetScriptDebugging()->SaveLuaDebugInfo(SLuaDebugInfo());

            // NOTE(botder): This is bad and HttpResponse::SetBody only accepts an int
            auto length = static_cast<int>(strJSON.size());

            if (length < 0)
                length = std::numeric_limits<int>::max();

            ipoHttpResponse->SetBody(strJSON.c_str(), length);
            return HTTPRESPONSECODE_200_OK;
        }

        ipoHttpResponse->SetBody("error: not found", 17);
        return HTTPRESPONSECODE_200_OK;
    }

    bool ResourceFileRouter::IsHttpAccessAllowed(CAccount* account)
    {
        CAccessControlListManager* pACLManager = g_pGame->GetACLManager();

        // New way
        // Check for "resource.<name>.http" being explicitly allowed
        if (pACLManager->CanObjectUseRight(account->GetName(), CAccessControlListGroupObject::OBJECT_TYPE_USER, m_resource.m_strResourceName + ".http",
                                           CAccessControlListRight::RIGHT_TYPE_RESOURCE, false))
        {
            return true;
        }

        // Old way phase 1
        // Check for "general.http" being explicitly denied
        if (!pACLManager->CanObjectUseRight(account->GetName(), CAccessControlListGroupObject::OBJECT_TYPE_USER, "http",
                                            CAccessControlListRight::RIGHT_TYPE_GENERAL, true))
        {
            return false;
        }
        // Check for "resource.<name>" being explicitly denied
        if (!pACLManager->CanObjectUseRight(account->GetName(), CAccessControlListGroupObject::OBJECT_TYPE_USER, m_resource.m_strResourceName,
                                            CAccessControlListRight::RIGHT_TYPE_RESOURCE, true))
        {
            return false;
        }

        // Old way phase 2
        // Check for "general.http" being explicitly allowed
        if (pACLManager->CanObjectUseRight(account->GetName(), CAccessControlListGroupObject::OBJECT_TYPE_USER, "http",
                                           CAccessControlListRight::RIGHT_TYPE_GENERAL, false))
        {
            return true;
        }
        // Check for "resource.<name>" being explicitly allowed
        if (pACLManager->CanObjectUseRight(account->GetName(), CAccessControlListGroupObject::OBJECT_TYPE_USER, m_resource.m_strResourceName,
                                           CAccessControlListRight::RIGHT_TYPE_RESOURCE, false))
        {
            return true;
        }

        // If nothing explicitly set, then default to denied
        return false;
    }
}            // namespace mtasa
