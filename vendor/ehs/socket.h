/*

EHS is a library for adding web server support to a C++ application
Copyright (C) 2001, 2002 Zac Hansen
  
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License only.
  
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
  
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
or see http://www.gnu.org/licenses/gpl.html

Zac Hansen ( xaxxon@slackworks.com )

*/

#pragma once

#include "networkabstraction.h"
#include <mtasa/IPSocket.h>
#include <mtasa/IPBindableEndpoint.h>

class Socket final : public NetworkAbstraction
{
public:
    Socket() = default;

    Socket(mtasa::IPSocket&& socket) : NetworkAbstraction(), m_socket(std::move(socket)), m_endpoint(socket.GetLocalEndpoint())
    {
    }

public:
    InitResult Init(int iPort, int inPort) override;
    bool       Create(const mtasa::IPBindableEndpoint& binding);
    void       Close() override { (void)m_socket.Close(); }
    
    int Read(void* ipBuffer, int ipBufferLength) override;
    int Send(const void* ipMessage, size_t inLength, int inFlags = 0) override;

    NetworkAbstraction* Accept() override;

    int  IsSecure() override { return 0; }
    bool IsReadable(int inTimeoutMilliseconds) override { return m_socket.IsReadable(inTimeoutMilliseconds); }
    bool IsWritable(int inTimeoutMilliseconds) override { return m_socket.IsWritable(inTimeoutMilliseconds); }
    bool IsAtError(int inTimeoutMilliseconds) override { return m_socket.IsErrored(inTimeoutMilliseconds); }
    
    int         GetFd() override { return m_socket.GetHandle(); }
    std::string GetAddress() override { return m_endpoint.GetAddress().ToString(); }
    int         GetPort() override { return m_endpoint.GetHostOrderPort(); }

private:
    mtasa::IPSocket   m_socket;
    mtasa::IPEndpoint m_endpoint;
};
