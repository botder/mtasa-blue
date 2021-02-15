/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Resource server-side file item class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CResourceFile.h"
#include "web/Response.h"

using namespace std::string_view_literals;

CResourceFile::CResourceFile(CResource* resource, const char* szShortName, const char* szResourceFileName, CXMLAttributes* xmlAttributes)
{
    m_strResourceFileName = szResourceFileName;

    // Stupid hack to automatically change all forward slashes to back slashes to get around internal http sub dir issue
    m_strShortName = szShortName;
    for (size_t i = 0; i < m_strShortName.size(); i++)
    {
        if (m_strShortName[i] == '\\')
        {
            m_strShortName[i] = '/';
        }
    }

    m_strWindowsName = m_strShortName;
    for (size_t i = 0; i < m_strWindowsName.size(); i++)
    {
        if (m_strWindowsName[i] == '/')
        {
            m_strWindowsName[i] = '\\';
        }
    }

    m_resource = resource;
    m_pVM = NULL;

    // Create a map of the attributes for later use
    if (xmlAttributes)
        for (list<CXMLAttribute*>::iterator iter = xmlAttributes->ListBegin(); iter != xmlAttributes->ListEnd(); iter++)
            m_attributeMap[(*iter)->GetName()] = (*iter)->GetValue();
}

bool CResourceFile::ProcessRequest(const Request& request, Response& response, mtasa::AuxiliaryMiddlewarePayload& payload)
{
    SString strDstFilePath = GetCachedPathFilename();

    FILE* file = File::Fopen(strDstFilePath.c_str(), "rb");

    if (file)
    {
        response.SetBody(std::move(strDstFilePath), true);
        fclose(file);
    }
    else
    {
        file = File::Fopen(m_strResourceFileName.c_str(), "rb");

        if (file)
        {
            response.SetBody(m_strResourceFileName, true);
            fclose(file);
        }
    }

    if (response.GetBody().empty())
    {
        response.SetStatusCode(500);
        response.SetBody("Can't read file!"sv);
        return false;
    }

    response.SetStatusCode(200);
    return true;
}

SString CResourceFile::GetCachedPathFilename(bool bForceClientCachePath)
{
    if (IsNoClientCache() == false || bForceClientCachePath)
        return PathJoin(g_pServerInterface->GetServerModPath(), "resource-cache", "http-client-files", m_resource->GetName(), GetName());
    else
        return PathJoin(g_pServerInterface->GetServerModPath(), "resource-cache", "http-client-files-no-client-cache", m_resource->GetName(), GetName());
}
