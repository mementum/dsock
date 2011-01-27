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

#ifndef __DSOCK_SOCKETHANDLE_HPP__
#define __DSOCK_SOCKETHANDLE_HPP__

#include <dsock/dsockdefs.hpp>

namespace dsock {

  class DSOCK_EXPORT SocketHandle_t {
  public:
    static const SocketHandle_t VoidHandle;
    static const SocketHandle_t NullHandle;

  protected:
    SocketHandleType_t m_handle;

  public:
    // Default copy constructor
    // Default assignment operator

    // Default constructor: This one takes the place of ... because
    // of the default parameter
    // Constructor: From Underlying type SocketHandleType_t
    SocketHandle_t( const SocketHandleType_t &p_handle = INVALID_SOCKET) {
      *this = p_handle;
    }

    // Assignment: From Underlying type SocketHandleType_t
    SocketHandle_t &operator =( const SocketHandleType_t &p_handle) {
      m_handle = p_handle;
      return *this;
    }

    const SocketHandleType_t &
    GetHandle( void) const {
      return m_handle;
    }

    // Conversion: To Underlying type SocketHandleType_t
    operator const SocketHandleType_t &() const {
      return m_handle;
    }
    //operator SocketHandleType_t &() const { return m_handle;}

    // Operators: Comparison and equality
    // So we can use this as an integral type and can
    // use STL containers
    bool operator <( const SocketHandle_t p_sockethandle) const {
      return m_handle < p_sockethandle.m_handle;
    }
    bool operator ==( const SocketHandle_t p_sockethandle) const {
      return m_handle == p_sockethandle.m_handle;
    }

    // Utility functions to test for validity, though the
    // validity lies in the operating system itself
    bool operator !( void) const {
      return this->IsValid() == false;
      //return m_handle == SocketHandle_t::VoidHandle;
    }
    bool IsValid( void) const {
      return m_handle != SocketHandle_t::VoidHandle;
    }
  };
}

#endif
