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

#ifndef __DSOCK_TASK_HPP__
#define __DSOCK_TASK_HPP__

#include <vector>
#include <map>
using namespace std;

#include <dsock/dsockdefs.hpp>
#include <dsock/socket.hpp>
#include <dsock/alarm.hpp>

namespace dsock {

  class App_t;

  class Task_t {
  protected:
    App_t *m_owner;

    vector<Io_t> m_iolist;
    vector<Alarm_t> m_period;
    map<Io_t, pair<char *, int> > m_buf;

  public:

    Task_t( long p_period = 0);
    Task_t( const Io_t &p_io, long p_period = 0);

    virtual ~Task_t( void);

    virtual void SetOwner( const App_t &p_app);
    virtual App_t &GetOwner( void);
    virtual const App_t *Owner( void) const;

    virtual Io_t &AddIo( const Io_t &p_io, size_t p_buflen = 65536);
    virtual void DelIo( const Io_t &p_io);

    virtual const Alarm_t &AddPeriod( const Alarm_t &p_alarm,
				      bool p_periodic = true);
    virtual void DelPeriod( const Alarm_t &p_alarm);

    virtual const Alarm_t &AddTimeout( const Alarm_t &p_alarm);
    virtual void DelTimeout( const Alarm_t &p_alarm);

    virtual void IoRead( const Io_t &p_io);
    virtual void IoData( const Io_t &p_io, const char *p_buf, size_t p_buflen);
    virtual void IoOpen( const Io_t &p_io);
    virtual void IoClose( const Io_t &p_io);
    virtual void IoException( const Io_t &p_io);

    virtual void HeartBeat( const Alarm_t &p_alarm);

    virtual void Timeout( const Alarm_t &p_alarm);

    virtual const char *ClassName(void) const { return "Task_t"; };

    friend class App_t;

  };

};


#endif
