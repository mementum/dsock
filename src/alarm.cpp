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

#include <dsock/alarm.hpp>

namespace dsock {

  const TimeVal_t TimeVal_t::ZeroTime;

  TimeVal_t::TimeVal_t( long p_sec, long p_usec)
    : m_tv_sec( p_sec), m_tv_usec( p_usec) {
  }

  TimeVal_t::TimeVal_t( long p_msec)
    : m_tv_sec( p_msec / 1000), m_tv_usec( (p_msec % 1000) * 1000) {
  }

  TimeVal_t::TimeVal_t( const TimeVal_t &p_timeval) {
    m_tv_sec = p_timeval.m_tv_sec;
    m_tv_usec = p_timeval.m_tv_usec;
  }

  TimeVal_t::TimeVal_t( const struct timeval &p_timeval) {
    m_tv_sec = p_timeval.tv_sec;
    m_tv_usec = p_timeval.tv_usec;
  }

  TimeVal_t &
  TimeVal_t::operator =( const TimeVal_t &p_timeval) {
    m_tv_sec = p_timeval.m_tv_sec;
    m_tv_usec = p_timeval.m_tv_usec;
    return *this;
  }

  TimeVal_t &
  TimeVal_t::operator =( long p_msec) {
    m_tv_sec = p_msec / 1000;
    m_tv_usec = ( p_msec % 1000) * 1000;
    return *this;
  }

  bool
  TimeVal_t::operator <( const TimeVal_t &p_timeval) const {
    return ( m_tv_sec < p_timeval.m_tv_sec) ||
      ( m_tv_sec == p_timeval.m_tv_sec && m_tv_usec < p_timeval.m_tv_usec);
  }

  bool
  TimeVal_t::operator ==( const TimeVal_t &p_timeval) const {
    return m_tv_sec == p_timeval.m_tv_sec && m_tv_usec == p_timeval.m_tv_usec;
  }

  bool
  TimeVal_t::operator !( void) const {
    return m_tv_sec == 0 && m_tv_usec == 0;
  }
  
  TimeVal_t &
  TimeVal_t::operator -=( const TimeVal_t &p_timeval) {
    m_tv_sec -= p_timeval.m_tv_sec;
    m_tv_usec -= p_timeval.m_tv_usec;
    if( m_tv_usec < 0) {
      m_tv_sec--;
      m_tv_usec += 1000000;
    }
    return *this;
  }

  TimeVal_t &
  TimeVal_t::operator -=( long p_msec) {
    m_tv_sec -= p_msec / 1000;
    m_tv_usec -= ( p_msec % 1000) * 1000;
    if( m_tv_usec < 0) {
      m_tv_sec--;
      m_tv_usec += 1000000;
    }
    return *this;
  }

  TimeVal_t &
  TimeVal_t::operator +=( const TimeVal_t &p_timeval) {
    m_tv_sec += p_timeval.m_tv_sec;
    m_tv_usec += p_timeval.m_tv_usec;
    if( m_tv_usec >= 1000000) {
      m_tv_sec++;
      m_tv_usec -= 1000000;
    }
    return *this;
  };

  TimeVal_t &
  TimeVal_t::operator +=( long p_msec) {
    m_tv_sec += p_msec / 1000;
    m_tv_usec += ( p_msec % 1000) * 1000;
    if( m_tv_usec >= 1000000) {
      m_tv_sec++;
      m_tv_usec -= 1000000;
    }
    return *this;
  };

  TimeVal_t &
  TimeVal_t::operator <<( long p_msec) {
    return *this = p_msec;
  }

  TimeVal_t &
  TimeVal_t::operator >>( long &p_msec) {
    p_msec = this->MSeconds();
    return *this;
  }

  void
  TimeVal_t::GetTimeOfDay( void) {
    struct timeval l_tv;
    ::gettimeofday( &l_tv, NULL);
    m_tv_sec = l_tv.tv_sec;
    m_tv_usec = l_tv.tv_usec;
  }

  long
  TimeVal_t::Seconds( void) const {
    return m_tv_sec + m_tv_usec / 1000000;
  }

  long
  TimeVal_t::MSeconds( void) const {
    return m_tv_sec * 1000 + m_tv_usec / 1000;
  }

  long
  TimeVal_t::USeconds( void) const {
    return m_tv_sec * 1000000 + m_tv_usec;
  }

  void
  TimeVal_t::SetTimeVal( struct timeval &p_tv) {
    p_tv.tv_sec = m_tv_sec;
    p_tv.tv_usec = m_tv_usec;
  }

  TimeVal_t
  operator -( const TimeVal_t &p_t1, const TimeVal_t &p_t2) {
    TimeVal_t l_timeval( p_t1);
    return l_timeval -= p_t2;
  }

  TimeVal_t
  operator +( const TimeVal_t &p_t1, const TimeVal_t &p_t2) {
    TimeVal_t l_timeval( p_t1);
    return l_timeval += p_t2;
  }

  //////////////////////////////////////////////////////////////

  Alarm_t::alarmmap_t Alarm_t::sm_idmap;

  unsigned int Alarm_t::sm_idpool = 0;

  const Alarm_t Alarm_t::ZeroAlarm;

  Alarm_t::Alarm_t( long p_msec, bool p_periodic) {
    this->Open( p_msec, p_periodic);
  }
  Alarm_t::Alarm_t( const TimeVal_t &p_timeval, bool p_periodic) {
    this->Open( p_timeval, p_periodic);
  }

  Alarm_t::Alarm_t( const Alarm_t &p_alarm) {
    m_id = p_alarm.m_id;
    m_it = p_alarm.m_it;
    (*m_it).second.m_refcount++;
  }

  Alarm_t::~Alarm_t() {
    this->Close();
  }

  void
  Alarm_t::Open( const TimeVal_t &p_timeval, bool p_periodic) {
    alarmmap_t::iterator l_it;
    do {
      m_id = sm_idpool++;
      l_it = sm_idmap.find( m_id);
    } while( l_it != sm_idmap.end());

    pair<alarmmap_t::iterator, bool>
      l_pit = sm_idmap.insert( make_pair( m_id,
					  AlarmData_t( p_timeval, p_periodic)));
    m_it = l_pit.first;
  }

  void
  Alarm_t::Close( void) {
    // decrease refcount for alarm and remove from
    // map if needed
    (*m_it).second.m_refcount--;
    if( (*m_it).second.m_refcount < 1) {
      sm_idmap.erase( m_it);
    }
  }

  Alarm_t &
  Alarm_t::operator =( const Alarm_t &p_alarm) {
    this->Close();

    m_id = p_alarm.m_id;
    m_it = p_alarm.m_it;
    (*m_it).second.m_refcount++;

    return *this;
  }

  void
  Alarm_t::Set( long p_msec) {
    (*m_it).second.m_tv = (*m_it).second.m_tvorig = p_msec;
  }

  void
  Alarm_t::Set( const TimeVal_t &p_timeval) {
    (*m_it).second.m_tv = (*m_it).second.m_tvorig = p_timeval;
  }

  void
  Alarm_t::Reset( void) {
    (*m_it).second.m_tv = (*m_it).second.m_tvorig;
  }

  void
  Alarm_t::ZeroIt( void) {
    (*m_it).second.m_tv = 0;
  }

  unsigned int
  Alarm_t::Id( void) const {
    return m_id;
  }

  bool
  Alarm_t::IsZero( void) const {
    return !((*m_it).second.m_tv);
  }

  void
  Alarm_t::SetTimeVal( struct timeval &p_timeval) {
    (*m_it).second.m_tv.SetTimeVal( p_timeval);
  }

  bool
  Alarm_t::Periodic( void) const {
    return (*m_it).second.m_periodic;
  }

  void
  Alarm_t::SetPeriodic( bool p_periodic) {
    (*m_it).second.m_periodic = p_periodic;
  }

  bool
  Alarm_t::Served( void) const {
    return (*m_it).second.m_served;
  }

  void
  Alarm_t::SetServed( bool p_yesno) {
    (*m_it).second.m_served = p_yesno;
  }

  long
  Alarm_t::Seconds( void) const {
    return (*m_it).second.m_tv.Seconds();
  }

  long
  Alarm_t::USeconds( void) const {
    return (*m_it).second.m_tv.USeconds();
  }

  long
  Alarm_t::MSeconds( void) const {
    return (*m_it).second.m_tv.MSeconds();
  }

  long
  Alarm_t::OrigSeconds( void) const {
    return (*m_it).second.m_tvorig.Seconds();
  }

  long
  Alarm_t::OrigUSeconds( void) const {
    return (*m_it).second.m_tvorig.USeconds();
  }

  long
  Alarm_t::OrigMSeconds( void) const {
    return (*m_it).second.m_tvorig.MSeconds();
  }

  bool
  Alarm_t::operator <( const Alarm_t &p_alarm) const {
    return (*m_it).second.m_tv < (*(p_alarm.m_it)).second.m_tv;
  }

  bool
  Alarm_t::operator ==( const Alarm_t &p_alarm) const {
    return m_id == p_alarm.m_id;
  }

  bool
  Alarm_t::operator !( void) const {
    return !((*m_it).second.m_tv);
  }

  Alarm_t &
  Alarm_t::operator -=( const Alarm_t &p_alarm) {
    (*m_it).second.m_tv -= (*(p_alarm.m_it)).second.m_tv;

    if( (*m_it).second.m_tv < TimeVal_t::ZeroTime) {
      (*m_it).second.m_tvextra = -((*m_it).second.m_tv.MSeconds());
      (*m_it).second.m_tv = 0;
    } else
      (*m_it).second.m_tvextra = 0;
    return *this;
  }

  Alarm_t &
  Alarm_t::operator -=( const TimeVal_t &p_timeval) {
    (*m_it).second.m_tv -= p_timeval;

    if( (*m_it).second.m_tv < TimeVal_t::ZeroTime) {
      (*m_it).second.m_tvextra = -((*m_it).second.m_tv.MSeconds());
      (*m_it).second.m_tv = 0;
    } else {
      (*m_it).second.m_tvextra = 0;
    }
    return *this;
  }

  Alarm_t &
  Alarm_t::operator +=( const Alarm_t &p_alarm) {
    (*m_it).second.m_tv += (*(p_alarm.m_it)).second.m_tv;
    return *this;
  }

  long
  Alarm_t::ExtraTime( void) const {
    return (*m_it).second.m_tvextra.MSeconds();
  }

};
