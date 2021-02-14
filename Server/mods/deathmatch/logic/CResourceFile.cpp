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
#include "HTTPServer.h"

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

CResourceFile::~CResourceFile()
{
}

bool CResourceFile::ProcessRequest(mtasa::HTTPRequest& request, mtasa::HTTPResponse& response)
{
    SString strDstFilePath = GetCachedPathFilename();

    FILE* file = File::Fopen(strDstFilePath.c_str(), "rb");

    if (file)
    {
        response.body = std::move(strDstFilePath);
        fclose(file);
    }
    else
    {
        file = File::Fopen(m_strResourceFileName.c_str(), "rb");

        if (file)
        {
            response.body = m_strResourceFileName;
            fclose(file);
        }
    }

    if (response.body.empty())
    {
        response.statusCode = 500;
        response.body = "Can't read file!";
        return false;
    }

    response.statusCode = 200;
    response.serveFile = true;
    return true;
}

SString CResourceFile::GetCachedPathFilename(bool bForceClientCachePath)
{
    if (IsNoClientCache() == false || bForceClientCachePath)
        return PathJoin(g_pServerInterface->GetServerModPath(), "resource-cache", "http-client-files", m_resource->GetName(), GetName());
    else
        return PathJoin(g_pServerInterface->GetServerModPath(), "resource-cache", "http-client-files-no-client-cache", m_resource->GetName(), GetName());
}
