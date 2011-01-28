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

#include <dsock/udpsocket.hpp>


namespace dsock {

  ssize_t
  UdpSocket_t::Read(char *p_buf, size_t p_len) {
    struct sockaddr_in l_sin;
    socklen_t l_sinlen = sizeof( l_sin);

    ssize_t l_retval =
      ::recvfrom( m_handle, p_buf, p_len, 0, (struct sockaddr *)&l_sin, &l_sinlen);
    if( l_retval == SOCKET_CONNCLOSED || l_retval == SOCKET_ERROR)
      return SOCKET_ERROR;

    m_remoteaddr = l_sin;

    return l_retval;
  }
};
