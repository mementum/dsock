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

#include <dsock/sockinit.hpp>

namespace dsock {

  SockInit_t::SockInit_t( void) {
    this->StartSockets();
    this->StartTiming();
  }
  SockInit_t::~SockInit_t() {
    this->EndTiming();
    this->EndSockets();
  }


  void
  SockInit_t::StartSockets( void) {
#ifdef __WIN32__
    WORD l_wVersionRequested;
    WSADATA l_wsaData;
    int l_err;
      
    l_wVersionRequested = MAKEWORD( 2, 2); // Major, Minor
 
    l_err = WSAStartup( l_wVersionRequested, &l_wsaData);


    // We could check the result of l_wsaData.wVersion
    // LOBYTE( l_wsaData.wVersion) is major number
    // HIBYTE( l_wsaData.wVersion) is minor number
    // We want at least Major 2 ...
    // WSA will give major 2 starting with Win98 and Windows NT 4
    // and above
    // But minor is something I haven't found information about
    if( l_err != 0) {
      /* Tell the user that we could not find a usable */
      /* WinSock DLL.                                  */
      return;
    }
#endif
  }

  void
  SockInit_t::EndSockets( void) {
#ifdef __WIN32__
    WSACleanup();
#endif
  }

  void
  SockInit_t::EndTiming( void) {
#ifdef __WIN32__
    ::timeEndPeriod( m_wTimerRes);
#endif
  }

  void
  SockInit_t::StartTiming( void) {
#ifdef __WIN32__
#if 0
    const size_t TARGET_RESOLUTION = 1;// 1-millisecond target resolution

    TIMECAPS l_tc;
    UINT l_wTimerRes;

    if( ::timeGetDevCaps( &l_tc, sizeof( TIMECAPS)) != TIMERR_NOERROR) {
      // Error; application can't continue.
    }

    l_wTimerRes = ::min( ::max( l_tc.wPeriodMin, TARGET_RESOLUTION), l_tc.wPeriodMax);

    m_wTimerRes = l_wTimerRes;
    ::timeBeginPeriod( l_wTimerRes); 
#else
#endif
    m_wTimerRes = 1;
    ::timeBeginPeriod( 1); 
#endif
  }

};
