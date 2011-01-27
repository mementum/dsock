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

#ifndef __DSOCK_INETADDR_HPP__
#define __DSOCK_INETADDR_HPP__

#include <sstream>
#include <string>
using namespace std;

#include <dsock/dsockdefs.hpp>

namespace dsock {

  class DSOCK_EXPORT InetAddr_t {

  protected:
    // We could try deriving from "sockaddr_in"
    struct sockaddr_in m_sin;

    // This is only needed for "getprotobyname"
    //string m_protocol;

  public:
    const static InetAddr_t VoidAddr;
    const static InetAddr_t AnyAddr;

    virtual const char *ClassName( void) const { return "InetAddr_t";}

    InetAddr_t( const string &p_host = "", const string &p_service = "");
    InetAddr_t( const string &p_host, const IpPort_t &p_service);
    InetAddr_t( const IpAddr_t &p_host, const string &p_service);
    InetAddr_t( const IpAddr_t &p_host, const IpPort_t &p_service);

    virtual ~InetAddr_t();

    InetAddr_t( const sockaddr_in &p_sin);
    InetAddr_t &operator =( const sockaddr_in &p_sin);

  protected:
    virtual void Init( void);

  public:
    virtual const struct sockaddr *SockAddr( void) const;
    virtual const struct sockaddr_in *SinAddr( void) const;
    virtual socklen_t SinSize( void) const;

  public:
    virtual string GetAddrText( void) const;

    virtual IpAddr_t GetIpAddr( void) const;
    virtual string GetIpAddrText( void) const;
    virtual size_t GetIpAddrByte( size_t p_pos) const;

    virtual IpPort_t GetIpPort( void) const;
    virtual string GetIpPortText( void) const;
    virtual size_t GetIpPortByte( size_t p_pos) const;

  public:
    virtual bool SetIpAddr( const string &p_host);
    virtual bool SetIpAddr( const IpAddr_t &p_host);

    virtual bool SetIpPort( const string &p_service);
    virtual bool SetIpPort( const IpPort_t &p_service);

    virtual bool SetAddr( const string &p_host, const string &p_service);
    virtual bool SetAddr( const string &p_host, const IpPort_t &p_service);
    virtual bool SetAddr( const IpAddr_t &p_host, const string &p_service);
    virtual bool SetAddr( const IpAddr_t &p_host, const IpPort_t &p_service);

  public:
    virtual bool operator ==( const InetAddr_t &p_iaddr) const;
    virtual bool operator !=( const InetAddr_t &p_iaddr) const {
      return ( *this == p_iaddr) == false;
    }
    virtual bool operator !( void) const;
    virtual bool operator <( const InetAddr_t &p_iaddr) const;
  };

};

#endif
