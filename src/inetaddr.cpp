/*
  Dsock Socket Programming Library

  Copyright (C) 2001 Daniel J. Rodriguez aka Daniel J. Rodriksson

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
/*
  $Id: ftpclient.cpp,v 1.2 2006/09/26 15:44:43 RODRODDA Exp $
*/

#include <sstream>
using namespace std;

#include <dsock/inetaddr.hpp>

namespace dsock {

  const InetAddr_t InetAddr_t::VoidAddr( INADDR_NONE, INPORT_NONE);
  const InetAddr_t InetAddr_t::AnyAddr( INADDR_ANY, INPORT_ANY);

  InetAddr_t::InetAddr_t( const string &p_host, const string &p_service) {
    this->Init();
    if( this->SetAddr( p_host, p_service) == false)
      *this = InetAddr_t::VoidAddr;
  }

  InetAddr_t::InetAddr_t( const string &p_host, const IpPort_t &p_service) {
    this->Init();
    if( this->SetAddr( p_host, p_service) == false)
      *this = InetAddr_t::VoidAddr;
  }

  InetAddr_t::InetAddr_t( const IpAddr_t &p_host, const string &p_service) {
    this->Init();
    if( this->SetAddr( p_host, p_service) == false)
      *this = InetAddr_t::VoidAddr;
  }

  InetAddr_t::InetAddr_t( const IpAddr_t &p_host, const IpPort_t &p_service) {
    this->Init();
    if( this->SetAddr( p_host, p_service) == false)
      *this = InetAddr_t::VoidAddr;
  }

  InetAddr_t::~InetAddr_t() {
  }

  InetAddr_t::InetAddr_t( const sockaddr_in &p_sin) {
    *this = p_sin;
  }

  InetAddr_t &
  InetAddr_t::operator =( const sockaddr_in &p_sin) {
    // ::memcpy( &m_sin, &p_sin, sizeof( p_sin));
    m_sin.sin_family = p_sin.sin_family;
    m_sin.sin_addr.s_addr = p_sin.sin_addr.s_addr;
    m_sin.sin_port = p_sin.sin_port;
    return *this;
  }

  void
  InetAddr_t::Init( void) {
    // *this = InetAddr_t::VoidAddr;
    // ::memset( &m_sin, 0, sizeof( m_sin));
    m_sin.sin_family = AF_INET;
    m_sin.sin_addr.s_addr = INADDR_NONE;
    m_sin.sin_port = INPORT_NONE;
  }

  const struct sockaddr *
  InetAddr_t::SockAddr( void) const {
    return (const struct sockaddr *)&m_sin;
  }

  const struct sockaddr_in *
  InetAddr_t::SinAddr( void) const {
    return &m_sin;
  }

  socklen_t
  InetAddr_t::SinSize( void) const {
    return sizeof( m_sin);
  }

  string
  InetAddr_t::GetAddrText( void) const {
    return this->GetIpAddrText() + ":" + this->GetIpPortText();
  }

  IpAddr_t
  InetAddr_t::GetIpAddr( void) const {
    return ::ntohl( m_sin.sin_addr.s_addr);
  }

  string
  InetAddr_t::GetIpAddrText( void) const {
    stringstream l_sstream;
    l_sstream
      << this->GetIpAddrByte( 3) << "."
      << this->GetIpAddrByte( 2) << "."
      << this->GetIpAddrByte( 1) << "."
      << this->GetIpAddrByte( 0);
    return l_sstream.str();
  }

  size_t
  InetAddr_t::GetIpAddrByte( size_t p_pos) const {
    return ( ( this->GetIpAddr() >> ( p_pos * 8)) & 0xff);
  }

  IpPort_t
  InetAddr_t::GetIpPort( void) const {
    return ::ntohs( m_sin.sin_port);
  }

  string
  InetAddr_t::GetIpPortText( void) const {
    stringstream l_sstream;
    l_sstream << this->GetIpPort();
    return l_sstream.str();
  }

  size_t
  InetAddr_t::GetIpPortByte( size_t p_pos) const {
    return ( ( this->GetIpPort() >> ( p_pos * 8)) & 0xff);
  }

  bool
  InetAddr_t::SetIpAddr( const string &p_host) {
    if( p_host == "")
      return this->SetIpAddr( INADDR_ANY);

    struct hostent *l_phe = ::gethostbyname( p_host.c_str());
    if( l_phe != NULL) {
      ::memcpy( &m_sin.sin_addr, l_phe->h_addr, l_phe->h_length);
    } else {
      m_sin.sin_addr.s_addr = ::inet_addr( p_host.c_str());
    }

    return m_sin.sin_addr.s_addr != INADDR_NONE;
  }

  bool
  InetAddr_t::SetIpPort( const string &p_service) {
    if( p_service == "")
      return this->SetIpPort( INPORT_ANY);

    struct servent *l_pse =
      ::getservbyname( p_service.c_str(), NULL);

    if( l_pse != NULL)
      m_sin.sin_port = l_pse->s_port;
    else
      m_sin.sin_port = ::htons( (IpPort_t)::atoi( p_service.c_str())); 

    return m_sin.sin_port != INPORT_NONE;
  }

  bool
  InetAddr_t::SetIpAddr( const IpAddr_t &p_ipaddr) {
    m_sin.sin_addr.s_addr = ::htonl( p_ipaddr);
    return true;
  }

  bool
  InetAddr_t::SetIpPort( const IpPort_t &p_ipport) {
    m_sin.sin_port = ::htons( p_ipport);
    return true;
  }

  bool
  InetAddr_t::SetAddr( const string &p_host, const string &p_service) {
    return
      this->SetIpAddr( p_host) &&
      this->SetIpPort( p_service);
  }

  bool
  InetAddr_t::SetAddr( const string &p_host, const IpPort_t &p_service) {
    return
      this->SetIpAddr( p_host) &&
      this->SetIpPort( p_service);
  }
  
  bool
  InetAddr_t::SetAddr( const IpAddr_t &p_host, const string &p_service) {
    return
      this->SetIpAddr( p_host) &&
      this->SetIpPort( p_service);
  }

  bool
  InetAddr_t::SetAddr( const IpAddr_t &p_host, const IpPort_t &p_service) {
    return
      this->SetIpAddr( p_host) &&
      this->SetIpPort( p_service);
  }

  bool
  InetAddr_t::operator ==( const InetAddr_t &p_iaddr) const {
#if 1
    return
      m_sin.sin_family == p_iaddr.m_sin.sin_family &&
      m_sin.sin_addr.s_addr == p_iaddr.m_sin.sin_addr.s_addr &&
      m_sin.sin_port == p_iaddr.m_sin.sin_port;
#else
    return this->GetIpAddr() == p_iaddr.GetIpAddr() &&
      this->GetIpPort() == p_iaddr.GetIpPort();
#endif
  }

  bool
  InetAddr_t::operator !( void) const {
    return this->GetIpAddr() == (IpAddr_t)INADDR_NONE &&
      this->GetIpPort() == INPORT_NONE;
  }

  bool
  InetAddr_t::operator <( const InetAddr_t &p_iaddr) const {
    return ( this->GetIpAddr() < p_iaddr.GetIpAddr() ||
	     ( this->GetIpAddr() == p_iaddr.GetIpAddr() && this->GetIpPort() < p_iaddr.GetIpPort())
	     );
  }

};
