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

#include "socket.h"
#include "stats.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <cassert>
#endif

using namespace mtasa;

NetworkAbstraction::InitResult Socket::Init(int iPort, int inPort)
{
    (void)iPort;
    (void)inPort;
    return INITSOCKET_SUCCESS;
}

bool Socket::Create(const mtasa::IPAddressBinding& binding, std::uint16_t port)
{
    IPSocket socket{binding.address.GetAddressFamily(), IPSocketProtocol::TCP};

    if (!socket.Create() || !socket.SetAddressReuse(true) || !socket.SetNonBlocking(true))
        return false;

    if (socket.IsIPv6() && !socket.SetIPv6Only(binding.addressMode == IPAddressMode::IPv6Only))
        return false;

    if (!socket.Bind(IPEndpoint{binding.address, port}) || !socket.Listen(20))
        return false;

    m_socket = std::move(socket);
    return true;
}

int Socket::Read(void* ipBuffer, int ipBufferLength)
{
    std::string_view message = m_socket.Receive(reinterpret_cast<char*>(ipBuffer), static_cast<std::size_t>(ipBufferLength));
    return static_cast<int>(message.size());
}

int Socket::Send(const void* ipMessage, size_t inLength, int inFlags)
{
    StatsAddTotalBytesSent(inLength);
    return static_cast<int>(m_socket.Send(reinterpret_cast<const char*>(ipMessage), inLength));
}

NetworkAbstraction* Socket::Accept()
{
    std::optional<IPSocket> socket = m_socket.Accept();

    if (!socket.has_value() || !socket->SetNonBlocking(true))
        return nullptr;

    return new Socket(std::move(socket.value()));
}
