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

#ifndef __DSOCK_UDPSOCKET_HPP__
#define __DSOCK_UDPSOCKET_HPP__

#include <dsock/socket.hpp>

namespace dsock {

  class DSOCK_EXPORT UdpSocket_t : public Socket_t {
  public:
    virtual const char *ClassName(void) const { return "UdpSocket_t"; };

  public:
    UdpSocket_t( const SocketHandle_t &p_handle = SocketHandle_t::VoidHandle) :
      Socket_t( p_handle, "udp") {};

    UdpSocket_t( const InetAddr_t &p_iaddr, bool p_server = false) :
      Socket_t( p_iaddr, "udp", p_server) {}

    virtual ~UdpSocket_t( void) {}

    virtual ssize_t Read(char *p_buf, size_t p_len);
  };


  class DSOCK_EXPORT UdpClientSocket_t : public UdpSocket_t {
  public:
    virtual const char *ClassName(void) const { return "UdpClientSocket_t"; };
  public:
    UdpClientSocket_t( const SocketHandle_t &p_handle = SocketHandle_t::VoidHandle) :
      UdpSocket_t( p_handle) {};

    UdpClientSocket_t( const InetAddr_t &p_iaddr) :
      UdpSocket_t( p_iaddr, false) {}

    virtual ~UdpClientSocket_t( void) {}
  };

  class DSOCK_EXPORT UdpServerSocket_t : public UdpSocket_t {
  public:
    virtual const char *ClassName(void) const { return "UdpServerSocket_t"; };
  public:
    UdpServerSocket_t( const InetAddr_t &p_iaddr = InetAddr_t::AnyAddr) :
      UdpSocket_t( p_iaddr, true) {}

    virtual ~UdpServerSocket_t( void) {}
    
  };


};

#endif
