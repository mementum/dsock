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

#ifndef __DSOCK_ALARM_HPP__
#define __DSOCK_ALARM_HPP__

#include <map>
using namespace std;

#include <dsock/dsockdefs.hpp>


namespace dsock {

  class TimeVal_t {
  protected:
    long m_tv_sec;
    long m_tv_usec;
  public:
    const static TimeVal_t ZeroTime;
    TimeVal_t( long p_sec, long p_usec);
    TimeVal_t( long p_msec = 0);
    TimeVal_t( const TimeVal_t &p_timeval);
    TimeVal_t( const struct timeval &p_timeval);

    TimeVal_t &operator =( const TimeVal_t &p_timeval);
    TimeVal_t &operator =( long p_msec);

    bool operator <( const TimeVal_t &p_timeval) const;
    bool operator ==( const TimeVal_t &p_timeval) const;
    bool operator !( void) const;
    TimeVal_t &operator -=( const TimeVal_t &p_timeval);
    TimeVal_t &operator +=( const TimeVal_t &p_timeval);
    TimeVal_t &operator -=( long p_msec);
    TimeVal_t &operator +=( long p_msec);

    TimeVal_t &operator <<( long p_msec);
    TimeVal_t &operator >>( long &p_msec);
    void GetTimeOfDay( void);

    long Seconds( void) const;
    long MSeconds( void) const;
    long USeconds( void) const;

    long TvSeconds( void) const { return m_tv_sec;}
    long TvMSeconds( void) const { return m_tv_usec / 1000;}
    long TvUSeconds( void) const { return m_tv_usec;}

    void SetTimeVal( struct timeval &p_timeval);

  };

  TimeVal_t operator -( const TimeVal_t &p_t1, const TimeVal_t &p_t2);
  TimeVal_t operator +( const TimeVal_t &p_t1, const TimeVal_t &p_t2);

  ////////////////////////////////////////////////////////////////

  class Alarm_t {
  public:
    class AlarmData_t {
    public:
      TimeVal_t m_tv;
      TimeVal_t m_tvorig;
      TimeVal_t m_tvextra;
      bool m_periodic;
      bool m_served;
      int m_refcount;

      AlarmData_t( void) 
	: m_tv( 0), m_tvorig( 0), m_tvextra( 0),
	  m_periodic( false), m_served( true),
	  m_refcount( 1) {
      }
      AlarmData_t( const TimeVal_t &p_timeval = 0,
		   bool p_periodic = false,
		   bool p_served = false,
		   int p_refcount = 1)
	: m_tv( p_timeval), m_tvorig( p_timeval), m_tvextra( 0),
	  m_periodic( p_periodic), m_served( p_served),
	  m_refcount( p_refcount) {
      }
      AlarmData_t( long p_msec, 
		   bool p_periodic = false,
		   bool p_served = false,
		   int p_refcount = 1)
	: m_tv( p_msec), m_tvorig( p_msec), m_tvextra( 0),
	  m_periodic( p_periodic), m_served( p_served),
	  m_refcount( p_refcount) {
      }
      virtual AlarmData_t &operator =( const AlarmData_t &p_adata) {
	m_tv = p_adata.m_tv;
	m_tvorig = p_adata.m_tvorig;
	m_tvextra = p_adata.m_tvextra;
	m_periodic = p_adata.m_periodic;
	m_served = p_adata.m_served;
	m_refcount = p_adata.m_refcount;
	return *this;
      }

    };

  protected:
    typedef map<int, AlarmData_t> alarmmap_t;
    static alarmmap_t sm_idmap;
    static unsigned int sm_idpool;

    unsigned int m_id;
    alarmmap_t::iterator m_it;

  public:
    const static Alarm_t ZeroAlarm;

    Alarm_t( long p_msec = 0, bool p_periodic = false);
    Alarm_t( const Alarm_t &p_alarm);
    Alarm_t( const TimeVal_t &p_timeval, bool p_periodic = false);
    Alarm_t &operator =( const Alarm_t &p_alarm);

    virtual ~Alarm_t();

  protected:
    virtual void Open( const TimeVal_t &p_timeval, bool p_periodic);
    virtual void Close( void);

  public:

    virtual void Set( long p_msec);
    virtual void Set( const TimeVal_t &p_timeval);
    virtual void Reset( void);
    virtual void ZeroIt( void);

    virtual unsigned int Id( void) const;
    virtual bool IsZero( void) const;
    virtual void SetTimeVal( struct timeval &p_timeval);

    virtual bool Periodic( void) const;
    virtual void SetPeriodic( bool p_periodic);

    virtual bool Served( void) const;
    virtual void SetServed( bool p_yesno = true);

    virtual long Seconds( void) const;
    virtual long USeconds( void) const;
    virtual long MSeconds( void) const;

    virtual long OrigSeconds( void) const;
    virtual long OrigUSeconds( void) const;
    virtual long OrigMSeconds( void) const;

    virtual bool operator <( const Alarm_t &p_alarm) const;
    virtual bool operator ==( const Alarm_t &p_alarm) const;
    virtual bool operator !( void) const;

    virtual Alarm_t &operator -=( const Alarm_t &p_alarm);
    virtual Alarm_t &operator -=( const TimeVal_t &p_timeval);
    virtual Alarm_t &operator +=( const Alarm_t &p_alarm);

    virtual long ExtraTime( void) const;

  };

};

#endif
