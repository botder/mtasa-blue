/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Web server response
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <string>

namespace mtasa::web
{
    class Response
    {
    public:
        virtual ~Response() = default;

    public:
        const std::string& GetBody() const { return m_body; }
        const std::string& GetMime() const { return m_mime; }

        void SetBody(const std::string& body, bool isFilePath = false)
        {
            m_body = body;
            m_isBodyFilePath = isFilePath;
        }

        void SetBody(std::string&& body, bool isFilePath = false)
        {
            m_body = std::move(body);
            m_isBodyFilePath = isFilePath;
        }

        void SetBody(std::string_view body, bool isFilePath = false)
        {
            m_body = std::string(body);
            m_isBodyFilePath = isFilePath;
        }

        void SetBody(const char* body, bool isFilePath = false)
        {
            m_body = std::string(body);
            m_isBodyFilePath = isFilePath;
        }

        void SetMime(const std::string& mime) { m_mime = mime; }
        void SetMime(std::string&& mime) { m_mime = std::move(mime); }
        void SetMime(std::string_view mime) { m_mime = std::string(mime); }
        void SetMime(const char* mime) { m_mime = std::string(mime); }

        bool IsBodyFilePath() const { return m_isBodyFilePath; }

        virtual bool HasStatusCode() const { return false; } 
        virtual bool SetStatusCode(int code) { return false; }
        virtual bool TryGetStatusCode(int& code) { return false; }

        virtual bool HasHeaders() const { return false; }
        virtual bool SetHeader(std::string_view name, std::string_view value) = 0;

    private:
        std::string m_body;
        std::string m_mime = "application/octet-stream";
        bool        m_isBodyFilePath = false;
    };
}
