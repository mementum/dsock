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

#include <dsock/io.hpp>
#include <dsock/socket.hpp>

namespace dsock {

  io_t::iomap_t io_t::sm_iomap;

  io_t::io_t( int fd) {
    open( fd, false);
  }

  io_t::io_t(const io_t &io) {
    *this = io;
  }

  io_t::~io_t() {
    close();
  }

  void
  io_t::open( int fd, bool doclose) {
    if( doclose) // default is true
      close();
    // fd might or might not be there
    m_handle = fd;
    iomap_t::iterator it = sm_iomap.find( m_handle);
    if( it != sm_iomap.end()) {
      //(*it).second++; // sm_iomap[ m_handle]++;
      sm_iomap[ m_handle] = (*it).second + 1;
    } else {
      sm_iomap.insert( make_pair( m_handle, 1)); // sm_iomap[ m_handle] = 1;
    }
  }

  void
  io_t::close( void) {
#if 1
    iomap_t::iterator it = sm_iomap.find( m_handle);
    (*it).second--;
    //if( --sm_iomap[ m_handle] < 1)
    if( (*it).second < 1) {
      if( m_handle != -1) {
#ifdef __WIN32__
	//::shutdown( m_handle, SD_BOTH);
	::closesocket( m_handle);
#else
	::close( m_handle);
#endif
	sm_iomap.erase( m_handle);
      }
    }
#endif
  }

  io_t &
  io_t::operator =(const io_t &io) {
    open( io.m_handle, false);
    return *this;
  }

  int
  io_t::operator !( void) const {
    return m_handle == -1;
  }

  int
  io_t::operator ==( const io_t &io) const {
    return m_handle == io.m_handle;
  }

  int
  io_t::operator !=( const io_t &io) const {
    return m_handle != io.m_handle;
  }

  int
  io_t::operator <( const io_t &io) const {
    return m_handle < io.m_handle;
  }

  int
  io_t::operator >( const io_t &io) const {
    return m_handle > io.m_handle;
  }

  int
  io_t::operator <=( const io_t &io) const {
    return m_handle <= io.m_handle;
  }

  int
  io_t::operator >=( const io_t &io) const {
    return m_handle >= io.m_handle;
  }

  int
  io_t::handle( void) const {
    return m_handle;
  }

  int
  io_t::read( void *buf, int len) const {
    return ::read( m_handle, buf, len);
  }

  int
  io_t::write(const void *buf, int len) const {
    return ::write( m_handle, buf, len);
  }

};
