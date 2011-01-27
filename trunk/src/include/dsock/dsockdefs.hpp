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

#ifndef __DSOCK_DSOCKDEFS_HPP__
#define __DSOCK_DSOCKDEFS_HPP__

////////////////////////////////////////////////////////////
// Config definition file (autoconf/autoheader)
////////////////////////////////////////////////////////////
#if HAVE_CONFIG_H
#  include <config.h>
#endif

////////////////////////////////////////////////////////////
// Win32 DLLs Macros
////////////////////////////////////////////////////////////
#ifdef __WIN32__
#  ifdef DLL_EXPORT
#    define DSOCK_EXPORT    __declspec( dllexport)
#  else
#    ifdef DSOCK_DLL
#      define DSOCK_EXPORT  extern __declspec( dllimport)
#    else
#      define DSOCK_EXPORT
#    endif
#  endif
#else
#  define DSOCK_EXPORT
#endif

////////////////////////////////////////////////////////////
// Generic includes
////////////////////////////////////////////////////////////
#include <sys/types.h>
#include <stdint.h>
#include <sys/timeb.h>

////////////////////////////////////////////////////////////
// System dependent includes and functions per system
////////////////////////////////////////////////////////////
#ifndef __WIN32__

#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <netdb.h>
#  include <arpa/inet.h>

#else

#  include <windows.h>
#  include <winsock2.h>
#  include <ws2tcpip.h>

extern int gettimeofday( struct timeval *p_timeval, struct timezone *p_timezone);

#endif

namespace dsock {

  ////////////////////////////////////////////////////////////
  // Generic typedefs
  ////////////////////////////////////////////////////////////
  typedef int32_t IpAddr_t;
  typedef uint16_t IpPort_t;


#  define INPORT_NONE 0xffff
#  define INPORT_ANY 0

#ifndef __WIN32__
  ////////////////////////////////////////////////////////////
  // Non Win32 typedefs
  ////////////////////////////////////////////////////////////
  typedef unsigned int socklen_t;
  typedef const void * sockoptval_t;
  typedef  SocketHandleType_t int;

#define INVALID_SOCKET (SocketHandleType_t)(~0)

#  define SD_RECEIVE 0x00
#  define SD_SEND 0x01
#  define SD_BOTH 0x02

#  define closesocket close

#  define SOCKET_ERROR -1
#  define SOCKET_CONNCLOSED 0
#  define SOCKET_NOERROR 0

#  define errnosock errno

#else
  ////////////////////////////////////////////////////////////
  // Win32 typedefs
  ////////////////////////////////////////////////////////////
  // Already defined
  // typedef int socklen_t;
  typedef const char * sockoptval_t;
  typedef SOCKET SocketHandleType_t;


  // Already defined
  // #define INVALID_SOCKET (SocketHandleType_t)(~0)

#  define SHUT_RD 0x00
#  define SHUT_WR 0x01
#  define SHUT_RDWR 0x02

#  define errnosock WSAGetLastError()
#  define SOCKET_NOERROR 0
#  define SOCKET_CONNCLOSED 0

#endif
}

#include <iostream>
using namespace std;

#endif
