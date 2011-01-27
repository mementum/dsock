/*
  This file is part of DSock

  DSock is a C++ Network oriented multitasking library
  Copyright (C) 2001-2010  Daniel Rodriguez (aka Daniel Rodriksson)

  You can learn more and contact the author at:

    http://code.google.com/p/dsock/

  DSock is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  DSock is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with DSock. If not, see <http://www.gnu.org/licenses/>.
*/

#include <dsock/tcpsocket.hpp>


namespace dsock {

  TcpClientSocket_t
  TcpServerSocket_t::Accept( void) {
    struct sockaddr_in l_sin;
    socklen_t l_sinlen = sizeof( l_sin);

    SocketHandle_t l_handle = ::accept( m_handle,
					(struct sockaddr *)&l_sin,
					&l_sinlen);

    return TcpClientSocket_t( l_handle);
  }

};
