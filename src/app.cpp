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

#include <algorithm>
#include <string>
using namespace std;

#include <dsock/app.hpp>

namespace dsock {

  App_t::App_t( int p_argc, char **p_argv) {
    FD_ZERO( &m_fdset);
  }

  App_t::~App_t() {
    for( size_t l_i = 0; l_i < m_task.size(); l_i++)
      delete m_task[ l_i];
  }

  App_t &
  App_t::operator <<( const Task_t &p_task) {
    ((Task_t &)p_task).SetOwner( *this);
    m_task.push_back( (Task_t *)&p_task);
    this->AddIo( p_task);
    this->AddPeriod( p_task);
    return *this;
  }

  App_t &
  App_t::operator >>( const Task_t &p_task) {
    this->DelIo( p_task);
    this->DelPeriod( p_task);

    m_task.erase( ::std::find( m_task.begin(), m_task.end(), &p_task));

    return *this;
  }

  void
  App_t::AddIo( const Task_t &p_task) {
    for( size_t l_i = 0; l_i < p_task.m_iolist.size(); l_i++)
      this->AddIo( p_task.m_iolist[ l_i], p_task);
  }

  void
  App_t::DelIo( const Task_t &p_task) {
    for( size_t l_i = 0; l_i < p_task.m_iolist.size(); l_i++)
      this->DelIo( p_task.m_iolist[ l_i]);
  }

  void
  App_t::AddIo( const Io_t &p_io, const Task_t &p_task) {
    FD_SET( p_io.GetHandle(), &m_fdset);
    m_iotask[ p_io] = (Task_t *)&p_task;
  }

  void
  App_t::DelIo( const Io_t &p_io) {
    FD_CLR( p_io.GetHandle(), &m_fdset);
    m_iotask.erase( p_io);
  }

  void
  App_t::AddPeriod( const Task_t &p_task) {
    for( size_t l_i = 0; l_i < p_task.m_period.size(); l_i++) {
      this->AddPeriod( p_task.m_period[ l_i], p_task);
    }
  }

  void
  App_t::DelPeriod( const Task_t &p_task) {
    for( size_t l_i = 0; l_i < p_task.m_period.size(); l_i++)
      this->DelPeriod( p_task.m_period[ l_i]);
  }

  void
  App_t::AddPeriod( const Alarm_t &p_alarm, const Task_t &p_task) {
    m_alarmtask.insert( make_pair( p_alarm.Id(), (Task_t *)&p_task));
    m_alarminsert.push_front( p_alarm);
  }

  void
  App_t::DelPeriod( const Alarm_t &p_alarm) {
    m_alarmtask.erase( p_alarm.Id());
    m_alarm.remove( p_alarm);
  }

  void
  App_t::Run( void) {
    fd_set l_readset, l_exceptionset;
    fd_set *l_readsetptr, *l_exceptionsetptr;

    int l_retval;
    TimeVal_t l_spenttime, l_tvcurr, l_tvprev, l_extratime, l_selecttime;
    struct timeval *l_tvptr, l_tvselect;
    alarmlist_t::iterator l_alarmit;

    l_tvprev.GetTimeOfDay();
    while( true) {

      // prepare sockets ...
      ::memcpy( &l_readset, &m_fdset, sizeof( fd_set));
      ::memcpy( &l_exceptionset, &m_fdset, sizeof( fd_set));

      // Record time after looping
      l_tvcurr.GetTimeOfDay();

      // if the system has alarms set ...
      if( m_alarm.empty() == false) {

	l_spenttime = l_tvcurr - l_tvprev;

	// decrease alarms by time spent looping
	for( l_alarmit = m_alarm.begin(); l_alarmit != m_alarm.end(); l_alarmit++) {
	  *l_alarmit -= l_spenttime;
	}

	// was the front alarm serverd ? (it may have been removed)
	if( m_alarm.front().Served() == true) {
	  // Extra time accounts for the delay in serving the alarm
	  // So we should not wait that time anymore
	  // This could be negative as we could have been sleeping
	  // less time ...

	  if( m_alarm.front().MSeconds() > 0) {
	    // Served and some mseconds left, we spent too little
	    // time sleeping
	    l_extratime = -m_alarm.front().MSeconds();
	  } else {
	    // We spent too much time sleeping
	    l_extratime = m_alarm.front().ExtraTime();
	  }

 	  // alarm served - revert state and reset
	  m_alarm.front().SetServed( false);
	  m_alarm.front().Reset();

	  // if served and not periodic - remove
	  if( m_alarm.front().Periodic() == false)
	    m_alarm.pop_front();
	}
      }
      // If any new alarm is to be inserted
      if( m_alarminsert.empty() == false) {
	// Insert it ...
	m_alarm.merge( m_alarminsert);
      }

      if( m_alarm.empty() == false) {
      // new alarms may have been added - sort
	m_alarm.sort();

	// Wait the specified MilliSeconds minus the time we overspent
	// in serving the previous alarm
	l_selecttime = m_alarm.front().MSeconds() - l_extratime.MSeconds();
	if( l_selecttime < TimeVal_t::ZeroTime)
	  l_selecttime = TimeVal_t::ZeroTime;
	l_selecttime.SetTimeVal( l_tvselect);
	l_tvptr = &l_tvselect;
      } else {
	// No alarms, wait for a socket event "eternally"
	// Max time should be 31 days according to the opengroup documents
	l_tvptr = NULL;
      }

      // Record time after calculations as last time
      // FIXME: djr
      // Shouldn't it be better: l_tvprev.GetTimeOfDay();
      //l_tvprev = l_tvcurr;
      l_tvprev.GetTimeOfDay();
    
#if 0
      l_retval = ::select( 8*sizeof( l_readset),
			   &l_readset, NULL, &l_exceptionset,
			   l_tvptr);
#else
      size_t l_maxsocket;
      if( m_iotask.size() > 0) {
	l_maxsocket = Socket_t::MaxHandle().GetHandle();
	l_readsetptr = &l_readset;
	l_exceptionsetptr = &l_exceptionset;
      } else {
	l_maxsocket = 0;
	l_readsetptr = NULL;
	l_exceptionsetptr = NULL;
      }
      l_retval = ::select( Socket_t::MaxHandle().GetHandle(),
			   l_readsetptr, NULL, l_exceptionsetptr,
			   l_tvptr);
#endif

      // now handle return value
      if( l_retval < 0) {
	// An error happened, so let's do all the calculations
	// again
	continue;
      }

      if( l_retval == 0) {
	// Time out ... Alarm must be served
	Alarm_t &l_alarmfront = m_alarm.front();

	l_alarmfront.Reset();

	if( l_alarmfront.Periodic())
	  m_alarmtask[ l_alarmfront.Id()]->HeartBeat( l_alarmfront);
	else
	  m_alarmtask[ l_alarmfront.Id()]->Timeout( l_alarmfront);

	l_alarmfront.SetServed( true);

	continue;
      }

      IoHandle_t l_handle;
      iotaskmap_t::iterator l_ioit;
      for( l_ioit = m_iotask.begin(); l_ioit != m_iotask.end(); l_ioit++) {
	l_handle = (*l_ioit).first.GetHandle();

	if( FD_ISSET( l_handle, &l_exceptionset)) {
	  (*l_ioit).second->IoException( (*l_ioit).first);
	  break;
	}

	if( FD_ISSET( l_handle, &l_readset)) {
	  (*l_ioit).second->IoRead( (*l_ioit).first);
	  break;
	}
      }
    }
  }
}
