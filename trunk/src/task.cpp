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
using namespace std;

#include <dsock/dsockdefs.hpp>
#include <dsock/task.hpp>
#include <dsock/app.hpp>


namespace dsock {

  Task_t::Task_t( long p_period) : m_owner( NULL) {
    if( p_period > 0)
      this->AddPeriod( p_period);
  }

  Task_t::Task_t( const Io_t &p_io, long p_period) : m_owner( NULL) {
    this->AddIo( p_io);
    if( p_period > 0)
      this->AddPeriod( p_period);
  }

  Task_t::~Task_t( void) {
    if( this->Owner())
      this->GetOwner() >> *this;

    map<Io_t, pair<char *, int> >::iterator l_it = m_buf.begin();
    for( ; l_it != m_buf.end(); ++l_it) {
      delete [] (*l_it).second.first;
    }
  }

  void
  Task_t::SetOwner( const App_t &p_app) {
    m_owner = (App_t *)&p_app;
  }

  const App_t *
  Task_t::Owner( void) const {
    return m_owner;
  }

  App_t &
  Task_t::GetOwner( void) {
    return *m_owner;
  }

  Io_t &
  Task_t::AddIo( const Io_t &p_io, size_t p_buflen) {
    m_iolist.push_back( p_io);
    char *l_iobuf = NULL;
    if( p_buflen)
      l_iobuf = new char[ p_buflen];
    m_buf.insert( make_pair( p_io, make_pair( l_iobuf, p_buflen)));
    if( this->Owner())
      this->GetOwner().AddIo( p_io, *this);
    return (Io_t &)p_io;
  }

  void
  Task_t::DelIo( const Io_t &p_io) {
    if( this->Owner())
      this->GetOwner().DelIo( p_io);
    m_iolist.erase( ::std::find( m_iolist.begin(), m_iolist.end(), p_io));
  }

  const Alarm_t &
  Task_t::AddPeriod( const Alarm_t &p_alarm, bool p_periodic) {
    if( p_alarm.IsZero())
      return Alarm_t::ZeroAlarm;

    m_period.push_back( p_alarm);
    m_period[ m_period.size() -1 ].SetPeriodic( p_periodic);

    if( this->Owner())
      this->GetOwner().AddPeriod( p_alarm, *this);
    return m_period[ m_period.size() - 1];
  }

  void
  Task_t::DelPeriod( const Alarm_t &p_alarm) {
    if( this->Owner())
      this->GetOwner().DelPeriod( p_alarm);
    m_period.erase( ::std::find( m_period.begin(), m_period.end(), p_alarm));
  }

  const Alarm_t &
  Task_t::AddTimeout( const Alarm_t &p_alarm) {
    return this->AddPeriod( p_alarm, false);
  }

  void
  Task_t::DelTimeout( const Alarm_t &p_alarm) {
    this->DelPeriod( p_alarm);
  }

  void
  Task_t::IoRead( const Io_t &p_io) {
    char *l_buf = m_buf[ p_io].first;
    int l_buflen = m_buf[ p_io].second;

    l_buflen = ( (Io_t &)p_io).Read( l_buf, l_buflen);

    if( l_buflen == SOCKET_CONNCLOSED)
      this->IoClose( p_io);
    else if( l_buflen == SOCKET_ERROR)
      this->IoClose( p_io);
    else
      this->IoData( p_io, l_buf, l_buflen);
  }

  void
  Task_t::IoData( const Io_t &p_io, const char *, size_t) {
  }

  void
  Task_t::IoOpen( const Io_t &p_io) {
  }

  void
  Task_t::IoClose( const Io_t &p_io) {
    this->DelIo( p_io);
    ((Io_t &)p_io).Close();
  }

  void
  Task_t::IoException( const Io_t &p_io) {
    this->DelIo( p_io);
  }

  void
  Task_t::HeartBeat( const Alarm_t &) {
  }

  void
  Task_t::Timeout( const Alarm_t &) {
  }

};
