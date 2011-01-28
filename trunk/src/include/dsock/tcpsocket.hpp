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

#ifndef __DSOCK_TCPSOCKET_HPP__
#define __DSOCK_TCPSOCKET_HPP__

#include <dsock/socket.hpp>

namespace dsock {

  class DSOCK_EXPORT TcpSocket_t : public Socket_t {
  public:
    TcpSocket_t( const SocketHandle_t &p_handle = SocketHandle_t::VoidHandle) :
      Socket_t( p_handle, "tcp") {};

  public:
    virtual const char *ClassName(void) const { return "TcpSocket_t"; };

    TcpSocket_t( const InetAddr_t &p_iaddr, bool p_server = false) :
      Socket_t( p_iaddr, "tcp", p_server) {
    }

    virtual ~TcpSocket_t( void) {}

    virtual ssize_t Read(char *p_buf, size_t p_len);
    
    virtual bool TcpNoDelay( bool p_onoff) {
      return ::setsockopt( m_handle, IPPROTO_TCP, TCP_NODELAY,
			   (sockoptval_t)&p_onoff, sizeof( p_onoff)) != SOCKET_ERROR;
    }

  };


  class DSOCK_EXPORT TcpClientSocket_t : public TcpSocket_t {
  public:
    virtual const char *ClassName(void) const { return "TcpClientSocket_t"; };

  public:
    TcpClientSocket_t( const SocketHandle_t &p_handle = SocketHandle_t::VoidHandle) :
      TcpSocket_t( p_handle) {}

  public:
    TcpClientSocket_t( const InetAddr_t &p_iaddr) :
      TcpSocket_t( p_iaddr) {}

    virtual ~TcpClientSocket_t( void) {}
  };

  class DSOCK_EXPORT TcpServerSocket_t : public TcpSocket_t {
  public:
    virtual const char *ClassName(void) const { return "TcpServerSocket_t"; };

  public:
    TcpServerSocket_t( const InetAddr_t &p_iaddr = InetAddr_t::AnyAddr,
		       ssize_t p_qlen = 32) :
      TcpSocket_t( p_iaddr, true) {
      if( this->Listen( p_qlen) == false)
	*this = (const TcpServerSocket_t &)Socket_t::VoidSocket;
    }

    virtual ~TcpServerSocket_t( void) {}
    
    virtual bool Listen( ssize_t p_qlen) { return ::listen( m_handle, p_qlen) != SOCKET_ERROR;}

    virtual TcpClientSocket_t Accept( void);

  };


};

#endif
