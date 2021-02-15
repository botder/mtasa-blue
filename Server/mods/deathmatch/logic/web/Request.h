/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Web server request
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Protocol.h"
#include <string_view>
#include <vector>

namespace mtasa::web
{
    class Client;

    class Header final
    {
    public:
        std::string_view name;
        std::string_view value;
    };

    class Parameter final
    {
    public:
        std::string_view name;
        std::string_view value;
    };

    class Credentials final
    {
    public:
        std::string_view username;
        std::string_view password;
    };

    // See https://tools.ietf.org/html/rfc3986
    class URI final
    {
    public:
        std::string_view scheme;
        std::string_view userinfo;
        std::string_view hostname;
        std::string_view port;
        std::string_view path;
        std::string_view query;
        std::string_view fragment;

    public:
        std::string GetBaseAddress() const;
    };

    std::vector<Parameter> ParseQuery(std::string_view query);

    class Request
    {
    public:
        virtual ~Request() = default;

    public:
        virtual Client&       GetClient() = 0;
        virtual const Client& GetClient() const = 0;

        virtual std::string_view GetBody() const { return {}; }

        virtual bool               HasCredentials() const { return false; }
        virtual Credentials*       GetCredentials() { return nullptr; }
        virtual const Credentials* GetCredentials() const { return nullptr; }

        virtual bool       HasURI() const { return false; }
        virtual URI*       GetURI() { return nullptr; }
        virtual const URI* GetURI() const { return nullptr; }

        virtual bool             HasMethod() const { return false; }
        virtual std::string_view GetMethod() const { return {}; }

        virtual bool                       HasHeaders() const { return false; }
        virtual const std::vector<Header>* GetHeaders() const { return nullptr; }

    public:
        Protocol GetProtocol() const { return m_protocol; }

        bool TryGetHeader(std::string_view name, Header& out) const;

    protected:
        Protocol m_protocol = Protocol::UNKNOWN;
    };
}
