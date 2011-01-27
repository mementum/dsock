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

#include <iostream>
using namespace std;

#include <dsock/dsock.hpp>
using namespace dsock;

class EchoServerTask_t : public Task_t {
protected:
  int m_periodicity;
  TimeVal_t l_prevtime, l_curtime;
  TimeVal_t l_difftime;

  size_t m_average;
  size_t m_count;
public:
  EchoServerTask_t( const Io_t &p_io, void *p_data) : Task_t( p_io, *(int *)(p_data)) {
    m_periodicity = *(int *)(p_data);
    m_average = 0;
    m_count = 0;
    cout << "Set period to: " << m_periodicity << endl;;
    l_prevtime.GetTimeOfDay();
  }

  virtual void HeartBeat( const Alarm_t &p_alarm) {
    cout << "EchoServerTask_t::Heartbeating" << endl;
    l_curtime.GetTimeOfDay();
    l_difftime = l_curtime; l_difftime -= l_prevtime;
    cout << "EchoServerTask_t::Heartbeating time from last beat (Milliseconds): " << l_difftime.MSeconds() << endl;
    cout << "EchoServerTask_t::Heartbeating Expected was: " << m_periodicity << endl;
    l_prevtime = l_curtime;

    size_t m_total;
    if( m_count > 0)
      m_total = m_average * m_count;
    else
      m_total = 0;

    m_count++;
    m_total += l_difftime.MSeconds();
    m_average = m_total/m_count;

    cout << "EchoServerTask_t::Heartbeating Average is: " << m_average << " in :" << m_count << " beats" <<endl;

  }

  virtual void Timeout( const Alarm_t &p_alarm) {
    cout << "EchoServerTask_t: Timeout" << endl;
  }

  virtual void IoData( const Io_t &p_io, const char *p_buf, size_t p_buflen) {
    ( (Io_t &)p_io).Write( p_buf, p_buflen);
  }
};


int
main( int argc, char *argv[]) {

#if 1
  ////////////////////////////////////////////////////////////
  // Application sample
  ////////////////////////////////////////////////////////////
  App_t l_app( argc, argv);


  TcpServerSocket_t l_echosock;
  cout << "Echo Socket bound at: " << l_echosock.GetLocalIpPort() << endl;

  if( argc < 2) {
    cerr << "Needs an arg in milliseconds" << endl;
    exit( 1);
  }
  int l_value = ::atoi( argv [ 1]);

  TcpServerVoid_t< EchoServerTask_t>
    l_echoserver( l_echosock, &l_value);

  l_app << l_echoserver;

  l_app.Run();

  exit( 0);
#endif

#if 0
  ////////////////////////////////////////////////////////////
  // Ftp Sample
  ////////////////////////////////////////////////////////////
  ssize_t p_retcode;

  cout << "1" << endl;
  FtpClient_t l_ftp( InetAddr_t( "ftp.rediris.es", "21"));
  cout << "2" << endl;
  if( l_ftp == FtpClient_t::VoidSocket) {
    cerr << "Ftp: Failed init" << endl;
    exit( 1);
  }

  string l_msg;
  if( l_ftp.WaitResponse( l_msg, p_retcode) == false) {
    cerr << "Ftp: Failed to get initial message";
    exit( 1);
  }
  cout << "------------------------------" << endl;
  cout << l_msg;
  cout << "------------------------------" << endl;

  if( l_ftp.User( "PruebaClienteFtp", p_retcode) == false) {
    cerr << "Ftp: User failed" << endl;
    exit( 1);
  }
  cout << "User accepted with code: " << p_retcode << endl;

  if( l_ftp.Pass( "PruebaClienteFtp@", p_retcode) == false) {
    cerr << "Ftp: Pass failed" << endl;
    exit( 1);
  }
  cout << "Pass accepted with code: " << p_retcode << endl;

  TcpClientSocket_t l_dataconn;
  if( l_ftp.Pasv( l_dataconn, p_retcode) == false) {
    cerr << "Ftp:Pasv command failed" << endl;
    exit( 1);
  }

  cout << "Pasv accepted with code: " << p_retcode << endl;

  cout << "Data Conn established to: ";
  cout << l_dataconn.GetRemoteAddr().GetAddrText() << endl;
  cout << "Port Byte 1: " << l_dataconn.GetRemoteAddr().GetIpPortByte( 1) << endl;
  cout << "Port Byte 0: " << l_dataconn.GetRemoteAddr().GetIpPortByte( 0) << endl;

  exit( 0);
#endif

  exit( 0);
}
