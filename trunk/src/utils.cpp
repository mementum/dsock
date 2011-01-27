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

#include <dsock/dsockdefs.hpp>

#ifdef __WIN32__

#include <sys/timeb.h>

/*
  http://www.opengroup.org/onlinepubs/000095399/functions/gettimeofday.html

  DESCRIPTION
  The gettimeofday() function shall obtain the current time, expressed
  as seconds and microseconds since the Epoch, and store it in the timeval
  structure pointed to by tp. The resolution of the system clock is
  unspecified.

  If tzp is not a null pointer, the behavior is unspecified.

  RETURN VALUE

  The gettimeofday() function shall return 0 and no value shall be reserved
  to indicate an error.
*/

int
gettimeofday( struct timeval *p_timeval, struct timezone * /* p_timezone */) {
  if( p_timeval == NULL)
    return 0;

#if 1

  // The 64 bit version _ftime64 and _timeb64 could be used
  // if timeval.tv_sec could accomodate a 64 bit value
  struct _timeb l_timeb;
  ::_ftime( &l_timeb);

  p_timeval->tv_sec = l_timeb.time;
  p_timeval->tv_usec = l_timeb.millitm * 1000; // from ms to us

#else

  DWORD l_t;
  l_t = ::timeGetTime();
  p_timeval->tv_sec = l_t / 1000;
  p_timeval->tv_usec = ( l_t % 1000) * 1000; // Remainder msecs --> usecs

#endif
 
  return 0;
}

#endif
