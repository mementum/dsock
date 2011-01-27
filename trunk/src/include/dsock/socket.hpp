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

#ifndef __DSOCK_SOCKET_HPP__
#define __DSOCK_SOCKET_HPP__

#include <string>
#include <map>
using namespace std;

#include <dsock/dsockdefs.hpp>
#include <dsock/sockinit.hpp>
#include <dsock/inetaddr.hpp>
#include <dsock/sockethandle.hpp>

namespace dsock {

  class DSOCK_EXPORT Socket_t {
  protected:
    const static SockInit_t sm_sockinit;
  public:
    static
    SocketHandle_t MaxHandle( void) {
      iomap_t::reverse_iterator l_rit = sm_iomap.rbegin();
      if( l_rit != sm_iomap.rend())
	return SocketHandle_t::NullHandle;

      return (*l_rit).first;
    }
  public:
    const static Socket_t VoidSocket;

  protected:
    enum { e_defaultTimeout = 30 * 1000};

    typedef map<SocketHandle_t, int> iomap_t;
    static iomap_t sm_iomap;

  protected:
    SocketHandle_t m_handle;
    InetAddr_t m_localaddr;
    InetAddr_t m_remoteaddr;

    ssize_t m_error;

    bool m_server;
    size_t m_timeout;
    string m_protocol;

  public:
    const char *Version( void) const;
    virtual const char *ClassName( void) const { return "Socket_t"; };

  public:
    Socket_t( const SocketHandle_t &p_handle = SocketHandle_t::VoidHandle, const string &p_proto = "tcp");

  public:
    Socket_t( const InetAddr_t &p_srcaddr, const InetAddr_t &p_dstaddr, const string &p_proto = "tcp");
    Socket_t( const InetAddr_t &p_iaddr, const string &p_proto = "tcp", bool p_server = false);

    Socket_t( const Socket_t &p_socket) { *this = p_socket;}
    virtual ~Socket_t() { this->Close();}

    Socket_t &operator =( const Socket_t &p_socket);

  protected:
    virtual void Open( const SocketHandle_t &p_handle);
    virtual bool Init( void);
  public:
    virtual void Close( void);

    virtual bool Reconnect( const InetAddr_t &p_iaddr = InetAddr_t::VoidAddr);

    virtual bool Socket( const string &p_protocol);
    virtual bool Connect( const InetAddr_t &p_iaddr);
    virtual bool Bind( const InetAddr_t &p_iaddr);

    virtual bool GetSockName( void);
    virtual bool GetPeerName( void);

    virtual ssize_t Read( char *p_buf, size_t p_len);
    virtual ssize_t Write( const char *p_buf, size_t p_len);

    virtual IpAddr_t GetLocalIpAddr( void) const { return m_localaddr.GetIpAddr();}
    virtual IpAddr_t GetRemoteIpAddr( void) const { return m_remoteaddr.GetIpAddr();}

    virtual IpPort_t GetLocalIpPort( void) const { return m_localaddr.GetIpPort();}
    virtual IpPort_t GetRemoteIpPort( void) const { return m_remoteaddr.GetIpPort();}

    virtual const InetAddr_t &GetLocalAddr( void) const { return m_localaddr;}
    virtual const InetAddr_t &GetRemoteAddr( void) const { return m_remoteaddr;}

    virtual bool SetTimeout( size_t p_timeout);

    virtual bool operator !(void) const { return m_handle == SocketHandle_t::VoidHandle;}
    virtual bool operator !=( const Socket_t &p_socket) const { return ( *this == p_socket) == false;}
    virtual bool operator ==( const Socket_t &p_socket) const { return m_handle == p_socket.m_handle;}
    virtual bool operator <( const Socket_t &p_socket) const { return m_handle < p_socket.m_handle;}

    virtual const SocketHandle_t &GetHandle( void) const { return m_handle;}

  public:
    virtual ssize_t ReadLine( string &p_line);
#if 0    
    virtual ssize_t InputTimeout( void);
    virtual ssize_t OutputTimeout( void);
#endif

  };

  typedef Socket_t Io_t;
  typedef SocketHandle_t IoHandle_t;

};

#endif

