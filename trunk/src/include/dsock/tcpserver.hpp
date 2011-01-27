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

#ifndef __DSOCK_TCPSERVER_HPP__
#define __DSOCK_TCPSERVER_HPP__

#include <dsock/tcpsocket.hpp>
#include <dsock/task.hpp>

namespace dsock {

  template <class T, class P> class TcpServerBase_t : public Task_t {
  protected:
    P m_initdata;
    TcpServerSocket_t m_serverio;
  public:
    TcpServerBase_t(const TcpServerSocket_t &p_io, const P &p_initdata):
      Task_t( p_io), m_initdata( p_initdata), m_serverio( p_io) {
    };
    virtual ~TcpServerBase_t( void) {};

    virtual void IoRead( const Io_t &) {

      T *l_task = new T( m_serverio.Accept(), m_initdata);
      if( l_task != NULL) {
	if( this->Owner())
	  this->GetOwner() << *l_task;
      }
    };

    virtual const char *ClassName(void) const { return "<TcpServerBase_t>"; };
  };


  template <class T> class TcpServerVoid_t :
    public TcpServerBase_t< T, void *> {
  public:
    TcpServerVoid_t(const TcpServerSocket_t &p_io, void *p_initdata)
      : TcpServerBase_t<T, void *>( p_io, p_initdata) {
    };
    virtual ~TcpServerVoid_t( void) {};

    virtual const char *ClassName(void) const { return "<TcpServerVoid_t>"; };
  };


  template <class T> class TcpServerInet_t :
    public TcpServerBase_t< T, InetAddr_t> {
  public:
    TcpServerInet_t(const TcpServerSocket_t &p_io, const InetAddr_t &p_initdata)
      : TcpServerBase_t<T, InetAddr_t>( p_io, p_initdata) {
    };
    virtual ~TcpServerInet_t( void) {};

    virtual const char *ClassName( void) const { return "<TcpServerInet_t>"; };
  };

};


#endif
