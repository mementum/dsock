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

#ifndef __DSOCK_FTPCLIENT_HPP__

#include <string>
#include <vector>
#include <iostream>
using namespace std;

#include <cstdio>

#include <dsock/tcpsocket.hpp>

namespace dsock {

  class FtpClient_t : public TcpClientSocket_t {
  public:

    class FtpResponseLine_t {
    public:
      size_t m_code;
      string m_msg;
      bool m_final;
    public:
      FtpResponseLine_t( const string &p_line = "") {
	this->Parse( p_line);
      }
      void Parse( const string &p_line) {
	m_final = false;
	m_code = 500;

	stringstream l_sstream( p_line);

	// An additional check could be done to see if the
	// line really starts with a number, but this is a
	// quick implementation and servers are expected to
	// conform to the standard

	// Parse Numeric code
	l_sstream >> m_code;

	// Check next char, ' ' is final, '-' is not.
	if( l_sstream.get() == ' ')
	  m_final = true;
	::std::getline( l_sstream, m_msg);
      }
      bool IsFinal( void) const { return m_final;}
    };


  public:
    FtpClient_t( const InetAddr_t &p_taddr) :
      TcpClientSocket_t( p_taddr) {}
    
    FtpClient_t( const SocketHandle_t &p_handle = SocketHandle_t::VoidHandle) :
      TcpClientSocket_t( p_handle) {}
    
    virtual ~FtpClient_t( void) {}
    
    virtual const char *ClassName(void) const { return "FtpClient_t"; }

  public:
    virtual bool SendVerb( const string &p_verb, const string &p_param,
			   ssize_t &p_retcode);

    virtual bool SendVerb( const string &p_verb, const vector<string> &p_params,
			   ssize_t &p_retcode);
    
    virtual bool WaitResponse( ssize_t &p_retcode);
    virtual bool WaitResponse( string &p_str, ssize_t &p_retcode);
    virtual bool WaitResponse( ostream &p_is, ssize_t &p_retcode);

    virtual bool SendAndWait( const string &p_verb, const string &p_param,
			      ssize_t &p_retcode);
    virtual bool SendAndWait( const string &p_verb, const string &p_param,
			      ostream &p_os,
			      ssize_t &p_retcode);


    virtual bool User( const string &p_user, ssize_t &p_retcode);
    virtual bool Pass( const string &p_pass, ssize_t &p_retcode);

#if 0
    virtual bool Stor( const string &p_filename, const string &p_filedest,
		       istream &p_is, TcpClientSocket_t &p_dataconn,
		       ssize_t &p_retcode);
#else
    virtual bool Stor( const string &p_filename, const string &p_filedest,
		       FILE *p_is, TcpClientSocket_t &p_dataconn,
		       ssize_t &p_retcode);
#endif

    virtual bool Stor( const string &p_filename, const string &p_filedest,
		       TcpClientSocket_t &p_dataconn,
		       ssize_t &p_retcode);
    
    virtual bool Port( const IpPort_t &p_ipport, TcpClientSocket_t &p_dataconn,
		       ssize_t &p_retcode);

    virtual bool Port( const InetAddr_t &p_taddr, TcpClientSocket_t &p_dataconn,
		       ssize_t &p_retcode);

    virtual bool Pasv( TcpClientSocket_t &p_dataconn, ssize_t &p_retcode);
    

    virtual bool Type( const string &p_type,
		       ssize_t &p_retcode);

    virtual bool TypeBin( ssize_t &p_retcode);
    virtual bool TypeAscii( ssize_t &p_retcode);

    virtual bool Quit( ssize_t &p_retcode);

#if 0
    virtual bool Quote( const string &p_quote,
			ssize_t &p_retcode);

    virtual bool Quote( const vector<string> &p_quote,
			ssize_t &p_retcode);

    virtual bool Quote( const string &p_quoteverb, const vector<string> &p_quoteparams,
			ssize_t &p_retcode);

    virtual bool Quote( const string &p_quoteverb, const string &p_quoteparam,
			ssize_t &p_retcode);
#endif
  };

  ////////////////////////////////////////////////////////////
  // FtpClientAdam2_t
  ////////////////////////////////////////////////////////////

  class FtpClientAdam2_t : public FtpClient_t {

  public:

    FtpClientAdam2_t( const InetAddr_t &p_taddr);

    virtual ~FtpClientAdam2_t( void) {}

    virtual bool SetEnv( const string &p_var, const string &p_val,
			 ssize_t &p_retcode);

    virtual bool GetEnv( const string &p_var, string &p_val,
			 ssize_t &p_retcode);

    virtual bool Media( const string &p_media,
			ssize_t &p_retcode);
    virtual bool MediaFlash( ssize_t &p_retcode);
    virtual bool MediaSdram( ssize_t &p_retcode);

    virtual bool Reboot( ssize_t &p_retcode);

    virtual const char *ClassName(void) const { return "FtpClientAdam2_t"; };
  };

}



#endif
