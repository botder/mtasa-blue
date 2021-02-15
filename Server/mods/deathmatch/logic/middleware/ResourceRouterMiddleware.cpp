/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Middleware for resource routing
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "ResourceRouterMiddleware.h"
#include "CResource.h"
#include "web/Request.h"
#include "web/Response.h"

namespace mtasa
{
    using namespace mtasa::web;
    using namespace std::string_view_literals;

    bool ResourceRouterMiddleware::PreProcessRequest(const Request& request, Response& response, AuxiliaryMiddlewarePayload& payload)
    {
        const URI* uri = request.GetURI();

        if (uri == nullptr)
            return false;

        std::string_view path = uri->path;

        // Extract resource name from URI
        std::string resourceName;

        if (path.empty() || path == "/"sv)
        {
            path = {};
        }
        else
        {
            std::size_t position = path.find('/', 1);

            if (position == std::string_view::npos)
            {
                resourceName = path.substr(1, position);
                path = {};
            }
            else
            {
                resourceName = path.substr(1, position - 1);
                path = path.substr(position);
            }
        }

        if (resourceName.empty())
        {
            if (!m_defaultResourceName.empty())
            {
                response.SetStatusCode(302);
                response.SetHeader("Location"sv, SString("%s/%s/", uri->GetBaseAddress().c_str(), m_defaultResourceName.c_str()));
                response.SetBody(SString("<a href='/%s/'>This is the page you want</a>", m_defaultResourceName.c_str()));
                return false;
            }
            else
            {
                response.SetStatusCode(200);
                response.SetBody(SString(
                    "You haven't set a default resource in your configuration file. You can either do this or visit %s/<i>resourcename</i>/ to see a "
                    "specific resource.<br/><br/>Alternatively, the server may be still starting up, if so, please try again in a minute.",
                    uri->GetBaseAddress().c_str()));
                return false;
            }
        }

        if (path.empty())
        {
            response.SetStatusCode(302);
            response.SetHeader("Location"sv, SString("%s/%s/", uri->GetBaseAddress().c_str(), resourceName.c_str()));
            response.SetBody(SString("<a href='/%s/'>This is the page you want</a>", resourceName.c_str()));
            return false;
        }

        CResource* resource = g_pGame->GetResourceManager()->GetResource(resourceName.c_str());

        if (resource == nullptr)
        {
            response.SetStatusCode(404);
            return false;
        }

        response.SetHeader("MTA-Resource"sv, resourceName);

        if (path.substr(0, 6) == "/call/")
        {
            payload.relativeResourcePath = path.substr(6);
            return resource->ProcessCallRequest(request, response, payload);
        }
        else
        {
            payload.relativeResourcePath = path.substr(1);
            return resource->ProcessRequest(request, response, payload);
        }

        return true;
    }
}
