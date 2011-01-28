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

#include <dsock/socket.hpp>

namespace dsock {

  const SockInit_t Socket_t::sm_sockinit;
  const Socket_t Socket_t::VoidSocket;

  Socket_t::iomap_t Socket_t::sm_iomap;

  Socket_t::Socket_t(const SocketHandle_t &p_handle, const string &p_proto) {
    
    // Why not closing ?
    // Because this is a constructor and no handle is in place yet
    m_error = SOCKET_NOERROR;
    m_server = false;

    m_localaddr = InetAddr_t::AnyAddr;
    m_remoteaddr = InetAddr_t::VoidAddr;

    m_protocol = p_proto;
    // Default constructor yields a VoidHandle
    //m_handle = SocketHandle_t::VoidHandle;

    this->Open(p_handle);

    this->GetSockName();
    this->GetPeerName();

    this->SetTimeout(Socket_t::e_defaultTimeout);
  }

  Socket_t::Socket_t(const InetAddr_t &p_srcaddr, const InetAddr_t &p_dstaddr,
		      const string &p_proto) {

    m_error = SOCKET_NOERROR;

    m_server = false; // Any socket willing to have a dstaddr is ALWAYS a client

    m_localaddr = p_srcaddr;
    m_remoteaddr = p_dstaddr;

    m_protocol = p_proto;
    // Default Constructor yields a VoidHandle
    //m_handle = SocketHandle_t::VoidHandle;
    if(this->Init() == false) {
      this->Close(); // Making it a VoidSocket (m_handle == VoidHandle)
    }
  }

  Socket_t::Socket_t(const InetAddr_t &p_iaddr, const string &p_proto, bool p_server) {

    m_error = SOCKET_NOERROR;

    m_server = p_server;

    if(m_server == true) {
      m_localaddr = p_iaddr;
      m_remoteaddr = InetAddr_t::VoidAddr;
    } else {
      m_remoteaddr = p_iaddr;
      m_localaddr = InetAddr_t::AnyAddr;
    }

    m_protocol = p_proto;

    // Default Constructor yields a VoidHandle
    //m_handle = SocketHandle_t::VoidHandle;

    if(this->Init() == false)
      this->Close(); // Making it a VoidSocket (m_handle == VoidHandle)
  }

  Socket_t &
  Socket_t::operator =(const Socket_t &p_socket) {
    this->Open(p_socket.m_handle);

    m_server = p_socket.m_server;
    m_error = p_socket.m_error;
    m_timeout = p_socket.m_timeout;
    //this->SetTimeout(p_socket.m_timeout);
    m_localaddr = p_socket.m_localaddr;
    // this->GetSockName();
    m_remoteaddr = p_socket.m_remoteaddr;
    // this->GetPeerName();

    m_protocol = p_socket.m_protocol;
    return *this;
  }

  void
  Socket_t::Open(const SocketHandle_t &p_handle) {
    //if(p_doclose) // default is true
    this->Close();

    m_handle = p_handle;
    if(m_handle != SocketHandle_t::VoidHandle) {
      iomap_t::iterator l_it = sm_iomap.find(m_handle);
      if(l_it != sm_iomap.end()) {
	(*l_it).second++;
      } else {
	sm_iomap.insert(make_pair(m_handle, 1)); // sm_iomap[m_handle] = 1;
      }
    }
  }

  void
  Socket_t::Close(void) {
    if(m_handle != SocketHandle_t::VoidHandle) {
      iomap_t::iterator l_it = sm_iomap.find(m_handle);
      if(l_it != sm_iomap.end()) {
	(*l_it).second--;

	if((*l_it).second < 1) {
	  ::shutdown(m_handle, SD_BOTH);
	  ::closesocket(m_handle);
	  sm_iomap.erase(l_it);
	}
      }

      // Nullify this socket
      m_handle = SocketHandle_t::VoidHandle;
    }
  }

  bool
  Socket_t::Init(void) {

    if(this->Socket(m_protocol) == false)
      return false;

    int l_optval = 1;
    if(::setsockopt(m_handle, SOL_SOCKET, SO_REUSEADDR,
		      (sockoptval_t)&l_optval, sizeof(l_optval)) == SOCKET_ERROR)
      return false;

    if(m_server == true) {
      if(this->Bind(m_localaddr) == false)
      return false;
    }
    if(m_server == false) {
      if(this->Connect(m_remoteaddr) == false)
	return false;
    }

    // See the local address we've bound to
    bool l_retval = this->GetSockName();

    // If this is a client (server == false)
    // see the remote address we connected to
    if(l_retval == true && m_server == false)
      l_retval = this->GetPeerName();

    if(l_retval == true)
      l_retval = this->SetTimeout(Socket_t::e_defaultTimeout);

    return l_retval;
  }

  bool
  Socket_t::Reconnect(const InetAddr_t &p_iaddr) {
    // Check that it can connect
    if(m_server == true)
      return false;

    // Close current socket if needed
    this->Close();

    if(p_iaddr != InetAddr_t::VoidAddr)
      m_remoteaddr = p_iaddr;

    m_localaddr = InetAddr_t::AnyAddr;

    bool l_retval = this->Init();
    if(l_retval == false)
      this->Close();

    return l_retval;
  }

  bool
  Socket_t::Socket(const string &p_protocol) {
    int l_socktype;

    if(p_protocol == "tcp")
      l_socktype = SOCK_STREAM;
    else if(p_protocol == "udp")
      l_socktype = SOCK_DGRAM;
    else
      return false;

    struct protoent *l_ppe = ::getprotobyname(p_protocol.c_str());
    
    if(l_ppe == NULL) {
      return false;
    }

    SocketHandle_t l_handle = ::socket(PF_INET, l_socktype, l_ppe->p_proto);
    if(l_handle == SocketHandle_t::VoidHandle)
      return false;

    this->Open(l_handle);
    return l_handle != SocketHandle_t::VoidHandle;
  }

  bool
  Socket_t::Connect(const InetAddr_t &p_iaddr) {
    return ::connect(m_handle,
		      p_iaddr.SockAddr(),
		      p_iaddr.SinSize()) != SOCKET_ERROR;
  }

  bool
  Socket_t::Bind(const InetAddr_t &p_iaddr) {
    return ::bind(m_handle,
		   p_iaddr.SockAddr(),
		   p_iaddr.SinSize()) != SOCKET_ERROR;

  }

  bool
  Socket_t::GetSockName(void) {
    struct sockaddr_in l_sin;

    socklen_t l_sinlen = sizeof(l_sin);
    int retval = ::getsockname(m_handle,
				(struct sockaddr *)&l_sin, &l_sinlen);
    
    if(retval != SOCKET_ERROR)
      m_localaddr = InetAddr_t(l_sin);

    return retval != SOCKET_ERROR;
  }

  bool
  Socket_t::GetPeerName(void) {
    struct sockaddr_in l_sin;

    socklen_t l_sinlen = sizeof(l_sin);
    int retval = ::getpeername(m_handle,
				(struct sockaddr *)&l_sin, &l_sinlen);
    
    if(retval != SOCKET_ERROR)
      m_remoteaddr = InetAddr_t(l_sin);

    return retval != SOCKET_ERROR;
  }

  ssize_t
  Socket_t::Read(char *p_buf, size_t p_len) {
    return ::recv(m_handle, p_buf, p_len, 0);
  }

  ssize_t
  Socket_t::Write(const char *p_buf, size_t p_len) {
    return ::send(m_handle, p_buf, p_len, 0);
  }

  /*
    Returns:
    SOCKET_ERROR (-1) if any error was encountered
    0...n Size of the string

    0 Can't be the indication of "Connection Closed" just like
    Read, because there can be "empty" lines ("\r\n") and after
    stripping the line delimiters, size may be 0.
  */

  ssize_t
  Socket_t::ReadLine(string &p_line) {
    p_line.clear();

    // Read until we meet a '\n'
    while(true) {

      char l_c;
      ssize_t l_ret = this->Read(&l_c, sizeof(l_c));

      if(l_ret == SOCKET_ERROR)
	return SOCKET_ERROR;

      if(l_ret == SOCKET_CONNCLOSED)
	return SOCKET_ERROR;

      if(l_c != '\n') {
	p_line.append(sizeof(l_c), l_c);

      } else {

	// Clean the previous '\r' if present

	size_t l_linesize = p_line.size();

	if(l_linesize > 0) {
	  if(p_line.at(l_linesize - 1) == '\r')
	    l_linesize--;
	    p_line.resize(l_linesize);
	}
	return l_linesize;

      }
    }
    return SOCKET_ERROR;
  }

#if 0
  ssize_t
  Socket_t::InputTimeout(void) {

    struct timeval l_tv;

    l_tv.tv_sec = (m_timeout / 1000);
    l_tv.tv_usec = (m_timeout % 1000) * 1000;

    fd_set l_readfds;
    
    FD_ZERO(&l_readfds);
    FD_SET(m_handle, &l_readfds);

    // select will return
    // 0 for timeout
    // 1 ... n for number of sockets waiting with data
    // -1 (SOCKET_ERROR) if an error happened
    return ::select(m_handle + 1, &l_readfds, NULL, NULL, &l_tv);
  }

  ssize_t
  Socket_t::OutputTimeout(void) {

    struct timeval l_tv;

    l_tv.tv_sec = (m_timeout / 1000);
    l_tv.tv_usec = (m_timeout % 1000) * 1000;

    fd_set l_writefds;
    
    FD_ZERO(&l_writefds);
    FD_SET(m_handle, &l_writefds);

    // select will return
    // 0 for timeout
    // 1 ... n for number of sockets waiting with data
    // -1 (SOCKET_ERROR) if an error happened
    return ::select(m_handle + 1, NULL, &l_writefds, NULL, &l_tv);
  }
#endif

  bool
  Socket_t::SetTimeout(size_t p_timeout) {

    m_timeout = p_timeout;
    ::setsockopt(m_handle, SOL_SOCKET, SO_RCVTIMEO,
		  (sockoptval_t)&p_timeout, sizeof(p_timeout));
    ::setsockopt(m_handle, SOL_SOCKET, SO_SNDTIMEO,
		  (sockoptval_t)&p_timeout, sizeof(p_timeout));
    return true;
  }

};

