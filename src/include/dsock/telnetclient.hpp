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

#ifndef __TELNETCLIENT_HPP__
#define __TELNETCLIENT_HPP__

#include <string>
#include <vector>
#include <sstream>
#include <map>
using namespace std;

#include <dsock/tcpsocket.hpp>

namespace dsock {
  class DSOCK_EXPORT TelActionReaction_t {
  public:
    TelActionReaction_t( const string &p_expect, const string &p_answer);
    string m_expect;
    string m_answer;
  };
 
  class DSOCK_EXPORT TelScript_t : public vector< TelActionReaction_t> {
  public:
    TelScript_t() {}

    TelScript_t( const char *p_params[][ 2]);
    TelScript_t( const string &p_expect, const string &p_answer);

    void Add( const char *p_params[][ 2]);
    void Add( const string &p_expect, const string &p_answer);
    void Add( const TelActionReaction_t &p_actreact);
  };

  class DSOCK_EXPORT TelCommand_t {
  public:
    enum TelEnum_t {
      NOP = 241,
      IAC = 255,
      WILL = 251,
      WONT = 252,
      DO = 253,
      DONT = 254,

      ECHO = 1,
      SUPPRESS_GO_AHEAD = 3,
      REMOTE_FLOW_CONTROL = 33,
      DUMMY = NOP
    };

  public:
    int m_command;
    int m_option;

    TelCommand_t( int p_command, int p_option) : m_command( p_command), m_option( p_option) {}
    static string CommandString( int p_command, int p_option);
    string CommandString( void) const;
  };

  bool
  operator <( const TelCommand_t &p_lhs, const TelCommand_t &p_rhs);
  
  bool
  operator ==( const TelCommand_t &p_lhs, const TelCommand_t &p_rhs);

  class DSOCK_EXPORT TelnetClient_t : public TcpClientSocket_t {
  protected:
    typedef map<TelCommand_t, TelCommand_t::TelEnum_t> TelOptions_t;

    TelOptions_t m_options;

  public:

    TelnetClient_t( const InetAddr_t &p_iaddr);

    virtual ssize_t Read( char *p_buf, size_t p_len);

    virtual bool RunScript( const TelScript_t &p_Telscript, const string &p_initstr = "");

    virtual string ProcessCommand( int p_command, int p_option);
    virtual string ProcessCommand( const TelCommand_t &p_cmd);
  };


}

#endif
