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

#ifndef __DSOCK_SOCKINIT_HPP__
#define __DSOCK_SOCKINIT_HPP__

#include <dsock/dsockdefs.hpp>

namespace dsock {

  class SockInit_t {
  protected:
#ifdef __WIN32__
    UINT m_wTimerRes;
#endif
  public:
    SockInit_t( void);
    virtual ~SockInit_t();

    void StartSockets( void);
    void EndSockets( void);

    void StartTiming( void);
    void EndTiming( void);
  };
  
}
#endif
