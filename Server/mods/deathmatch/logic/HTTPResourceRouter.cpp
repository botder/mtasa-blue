/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Router to resources for HTTP requests
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "HTTPResourceRouter.h"
#include "CResource.h"

namespace mtasa
{
    bool HTTPResourceRouter::PreProcessRequest(HTTPRequest& request, HTTPResponse& response)
    {
        // Extract resource name from URI
        std::string resourceName;

        if (request.uri.empty() || request.uri == "/")
        {
            request.uri = {};
        }
        else
        {
            std::size_t position = request.uri.find('/', 1);

            if (position == std::string_view::npos)
            {
                resourceName = request.uri.substr(1, position);
                request.uri = {};
            }
            else
            {
                resourceName = request.uri.substr(1, position - 1);
                request.uri = request.uri.substr(position);
            }
        }

        if (resourceName.empty())
        {
            if (!m_defaultResourceName.empty())
            {
                response.statusCode = 302;
                response.headers["Location"] = SString("http://%s/%s/", std::string(request.host).c_str(), m_defaultResourceName.c_str());
                response.body = SString("<a href='/%s/'>This is the page you want</a>", m_defaultResourceName.c_str());
                return false;
            }
            else
            {
                response.statusCode = 200;
                response.body = SString(
                    "You haven't set a default resource in your configuration file. You can either do this or visit http://%s/<i>resourcename</i>/ to see a "
                    "specific resource.<br/><br/>Alternatively, the server may be still starting up, if so, please try again in a minute.",
                    std::string(request.host).c_str());
                return false;
            }
        }

        if (request.uri.substr(0, 1) != "/")
        {
            response.statusCode = 302;
            response.headers["Location"] = SString("http://%s/%s/", std::string(request.host).c_str(), resourceName.c_str());
            response.body = SString("<a href='/%s/'>This is the page you want</a>", resourceName.c_str());
            return false;
        }

        CResource* resource = g_pGame->GetResourceManager()->GetResource(resourceName.c_str());

        if (resource == nullptr)
        {
            response.statusCode = 404;
            return false;
        }

        if (request.uri.substr(0, 6) == "/call/")
        {
            request.uri = request.uri.substr(6);
            return resource->ProcessCallRequest(request, response);
        }
        else
        {
            request.uri = request.uri.substr(1);
            return resource->ProcessRequest(request, response);
        }
    }
}            // namespace mtasa
