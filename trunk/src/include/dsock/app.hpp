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

#ifndef __DSOCK_APP_HPP__
#define __DSOCK_APP_HPP__

#include <vector>
#include <list>
#include <map>
using namespace std;

#include <dsock/dsockdefs.hpp>
#include <dsock/socket.hpp>
#include <dsock/alarm.hpp>
#include <dsock/task.hpp>

namespace dsock {

  class App_t {
  protected:
    fd_set m_fdset;

    vector<Task_t *> m_task;

    typedef map<Io_t, Task_t *> iotaskmap_t;
    iotaskmap_t m_iotask;

    typedef list<Alarm_t> alarmlist_t;
    alarmlist_t m_alarm, m_alarminsert;

    map<unsigned int, Task_t *> m_alarmtask;

  public:
    App_t( int argc, char **argv);
    virtual ~App_t();

    virtual App_t &operator <<( const Task_t &p_task);
    virtual App_t &operator >>( const Task_t &p_task);

    virtual void AddIo( const Task_t &p_task);
    virtual void DelIo( const Task_t &p_task);

    virtual void AddIo( const Io_t &p_io, const Task_t &p_task);
    virtual void DelIo( const Io_t &p_io);

    virtual void AddPeriod( const Task_t &p_task);
    virtual void DelPeriod( const Task_t &p_task);

    virtual void AddPeriod( const Alarm_t &p_alarm, const Task_t &p_task);
    virtual void DelPeriod( const Alarm_t &p_alarm);

    virtual void Run( void);
  };
};

#endif
