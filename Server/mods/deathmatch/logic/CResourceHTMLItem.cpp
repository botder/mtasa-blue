/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Resource server-side HTML item class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CResourceHTMLItem.h"
#include "middleware/Middleware.h"
#include "web/Server.h"
#include "web/Request.h"
#include "web/Response.h"

using namespace std::string_view_literals;

extern CServerInterface* g_pServerInterface;
extern CGame*            g_pGame;

CResourceHTMLItem::CResourceHTMLItem(CResource* resource, const char* szShortName, const char* szResourceFileName, CXMLAttributes* xmlAttributes,
                                     bool bIsDefault, bool bIsRaw, bool bRestricted, bool bOOPEnabled)
    : CResourceFile(resource, szShortName, szResourceFileName, xmlAttributes)
{
    m_bIsRaw = bIsRaw;
    m_type = RESOURCE_FILE_TYPE_HTML;
    m_bDefault = bIsDefault;
    m_pVM = NULL;
    m_bIsBeingRequested = false;
    m_bRestricted = bRestricted;
    m_bOOPEnabled = bOOPEnabled;
}

CResourceHTMLItem::~CResourceHTMLItem()
{
    Stop();
}

void CResourceHTMLItem::ClearPageBuffer()
{
    m_strPageBuffer.clear();
}

void CResourceHTMLItem::SetResponseHeader(const char* szHeaderName, const char* szHeaderValue)
{
    if (m_httpResponse)
    {
        m_httpResponse->SetHeader(std::string_view(szHeaderName), std::string_view(szHeaderValue));
    }
}

void CResourceHTMLItem::SetResponseCode(int responseCode)
{
    if (m_httpResponse)
    {
        m_httpResponse->SetStatusCode(responseCode);
    }
}

void CResourceHTMLItem::SetResponseCookie(const char* szCookieName, const char* szCookieValue)
{
    // CookieParameters params;
    // Datum            data;
    // data = szCookieValue;
    // params[szCookieName] = data;
    // m_currentResponse->SetCookie(params);
}

bool CResourceHTMLItem::AppendToPageBuffer(const char* szText, size_t length)
{
    if (szText)
        m_strPageBuffer.append(szText, length);
    return true;
}

bool CResourceHTMLItem::Start()
{
    if (!m_bIsRaw)
    {
        // go through and search for <* or *>
        FILE* pFile = File::Fopen(m_strResourceFileName.c_str(), "r");
        if (!pFile)
            return false;

        bool        bInCode = false;
        bool        bJustStartedCodeBlock = false;
        bool        bIsShorthandCodeBlock = false;
        std::string strScript;
        strScript += "function renderPage ( requestHeaders, form, cookies, hostname, url, querystring, user, requestBody, method )\n";
        strScript += "\nhttpWrite ( \"";            // bit hacky, possibly can be terminated straight away
        unsigned char c;
        int           i = 0;
        while (!feof(pFile))
        {
            c = ReadChar(pFile);
            if (feof(pFile))
                break;

            if (bInCode == false)            // we're in a plain HTML section
            {
                if (c == '<' && !feof(pFile))
                {
                    c = ReadChar(pFile);
                    if (c == '*')            // we've found <*
                    {
                        bInCode = true;
                        bJustStartedCodeBlock = true;
                        strScript.append("\" )\n");            // add ") to the end to terminate our last non-code section
                    }
                    else
                    {            // we found < but not a *, so just output both characters we read
                        strScript += '<';
                        strScript += c;
                    }
                }
                else
                {
                    if (c == '\r')
                    {
                        strScript += "\\r";
                    }
                    else if (c == '\n')
                    {
                        strScript += "\\n";
                    }
                    else if (c == '\\')
                    {
                        strScript += "\\\\";
                    }
                    else if (c == '\"')
                    {
                        strScript += "\\\"";
                    }
                    else
                        strScript += c;
                }
            }
            else
            {            // we're in a code block
                if (c == '*' && !feof(pFile))
                {
                    c = ReadChar(pFile);
                    if (c == '>')            // we've found *>
                    {
                        bInCode = false;
                        if (bIsShorthandCodeBlock)
                        {
                            bIsShorthandCodeBlock = false;
                            strScript += ')';            // terminate the 'httpWrite' function
                        }
                        strScript.append("\nhttpWrite ( \"");            // add httpWrite ( " to start a new non-code section
                    }
                    else
                    {            // we found * but not a >, so just output both characters we read
                        strScript += '*';
                        strScript += c;
                    }
                }
                else if (c == '=' && bJustStartedCodeBlock)
                {
                    strScript.append("httpWrite ( ");
                    bIsShorthandCodeBlock = true;
                }
                else
                {
                    if (c != '\t' && c != ' ')            // we allow whitespace before the shorthand '=' sign
                        bJustStartedCodeBlock = false;
                    strScript += c;
                }
            }
            i++;
        }

        if (!bInCode)
            strScript.append("\" )\n");
        strScript.append("\nend");

        /*     FILE * debug = fopen ("debug.lua", "w" );
             fwrite ( m_szBuffer, 1, strlen(m_szBuffer), debug );
             fclose ( debug );*/

        m_pVM = g_pGame->GetLuaManager()->CreateVirtualMachine(m_resource, m_bOOPEnabled);
        m_pVM->LoadScript(strScript.c_str());
        m_pVM->SetResourceFile(this);
        m_pVM->RegisterHTMLDFunctions();

        fclose(pFile);

        GetMimeType(m_strResourceFileName.c_str());

        return true;
    }
    else
    {
        // its a raw page
        FILE* file = File::Fopen(m_strResourceFileName.c_str(), "rb");
        if (file)
        {
            GetMimeType(m_strResourceFileName.c_str());

            // don't actually read it here, it could be way too large
            fclose(file);
            return true;
        }
        return false;
    }
}

bool CResourceHTMLItem::ProcessRequest(const Request& request, Response& response, mtasa::AuxiliaryMiddlewarePayload& payload)
{
    using namespace mtasa::web;

    const URI* uri = request.GetURI();

    if (uri == nullptr)
        return false;

    if (!m_pVM)
        Start();

    if (m_bIsBeingRequested)
    {
        response.SetStatusCode(500);
        response.SetBody("Busy"sv);
        return false;
    }

    m_bIsBeingRequested = true;

    int statusCode = 200;
    
    if (!m_bIsRaw)
    {
        response.SetMime(m_strMime);

        CLuaArguments formData;

        std::vector<Parameter> parameters = ParseQuery(uri->query);

        for (const Parameter& parameter : parameters)
        {
            if (parameter.name.empty())
                break;

            formData.PushString(std::string(parameter.name));
            formData.PushString(Server::Decode(parameter.value, true));
        }

        // for (FormValueMap::iterator iter = ipoHttpRequest->oFormValueMap.begin(); iter != ipoHttpRequest->oFormValueMap.end(); iter++)
        // {
        //     formData.PushString((*iter).first.c_str());
        //     formData.PushString(((FormValue)(*iter).second).sBody.c_str());
        // }

        CLuaArguments cookies;
        // for (CookieMap::iterator iter = ipoHttpRequest->oCookieMap.begin(); iter != ipoHttpRequest->oCookieMap.end(); iter++)
        // {
        //     cookies.PushString((*iter).first.c_str());
        //     cookies.PushString((*iter).second.c_str());
        // }

        CLuaArguments headers;

        if (const std::vector<Header>* headerList = request.GetHeaders(); headerList != nullptr)
        {
            for (const Header& header : *headerList)
            {
                if (header.name.empty())
                    continue;

                headers.PushString(std::string(header.name));
                headers.PushString(std::string(header.value));
            }
        }

        std::stringstream hostname;

        if (!uri->hostname.empty())
        {
            hostname << uri->hostname;

            if (!uri->port.empty())
                hostname << ":" << uri->port;
        }

        CLuaArguments querystring(formData);
        CLuaArguments args;
        args.PushTable(&headers);                                         // requestHeaders
        args.PushTable(&formData);                                        // form
        args.PushTable(&cookies);                                         // cookies
        args.PushString(hostname.str());            // hostname
        args.PushString(std::string(uri->path));            // url
        args.PushTable(&querystring);                                     // querystring
        args.PushAccount(payload.account);
        args.PushString(std::string(request.GetBody()));            // requestBody
        args.PushString(std::string(request.GetMethod()));          // method

        m_httpResponse = &response;
        args.CallGlobal(m_pVM, "renderPage");
        m_httpResponse = nullptr;

        response.SetBody(std::move(m_strPageBuffer));
    }
    else
    {
        FILE* file = File::Fopen(m_strResourceFileName.c_str(), "rb");

        if (file)
        {
            fclose(file);
            response.SetMime(m_strMime);
            response.SetBody(m_strResourceFileName, true);
        }
        else
        {
            statusCode = 500;
            response.SetBody("Can't read file!"sv);
        }
    }

    m_bIsBeingRequested = false;
    response.SetStatusCode(statusCode);
    return statusCode == 200;
}

void CResourceHTMLItem::GetMimeType(const char* szFilename)
{
    const char* pExtn = strrchr(szFilename, '.');
    if (pExtn)
    {
        pExtn++;
        if (strcmp(pExtn, "css") == 0)
            m_strMime = "text/css";
        else if (strcmp(pExtn, "png") == 0)
            m_strMime = "image/png";
        else if (strcmp(pExtn, "gif") == 0)
            m_strMime = "image/gif";
        else if (strcmp(pExtn, "jpg") == 0 || strcmp(pExtn, "jpeg") == 0)
            m_strMime = "image/jpg";
        else if (strcmp(pExtn, "js") == 0)
            m_strMime = "text/javascript";
        else
            m_strMime = "text/html";
    }
    else
        m_strMime = "text/html";
}

bool CResourceHTMLItem::Stop()
{
    if (m_pVM)
    {
        // Delete the events on this VM
        g_pGame->GetMapManager()->GetRootElement()->DeleteEvents(m_pVM, true);

        g_pGame->GetLuaManager()->RemoveVirtualMachine(m_pVM);
    }

    m_pVM = NULL;
    return true;
}
