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

#include <dsock/telnetclient.hpp>

namespace dsock {

  TelActionReaction_t::TelActionReaction_t( const string &p_expect, const string &p_answer) {
    m_expect = p_expect;
    m_answer = p_answer;
  }
  
  TelScript_t::TelScript_t( const char *p_params[][ 2]) {
    this->Add( p_params);
  }
  TelScript_t::TelScript_t( const string &p_expect, const string &p_answer) {
    this->Add( p_expect, p_answer);
  }

  void
  TelScript_t::Add( const char *p_params[][ 2]) {
    int l_i = 0;
    while( p_params[ l_i][ 0] != NULL) {
      this->Add( p_params[ l_i][ 0], p_params[ l_i][ 1]);
      l_i++;
    }
  }

  void
  TelScript_t::Add( const string &p_expect, const string &p_answer) {
    this->Add( TelActionReaction_t( p_expect, p_answer));
  }

  void
  TelScript_t::Add( const TelActionReaction_t &p_actreact) {
    this->push_back( p_actreact);
  }

  string
  TelCommand_t::CommandString( int p_command, int p_option) {
    string l_cmd( 3, TelCommand_t::IAC);
    l_cmd[ 1] = p_command;
    l_cmd[ 2] = p_option;
    return l_cmd;
  }

  string
  TelCommand_t::CommandString( void) const {
    return TelCommand_t::CommandString( this->m_command, this->m_option);
  }

  bool
  operator <( const TelCommand_t &p_lhs, const TelCommand_t &p_rhs) {
    if( p_lhs.m_option < p_rhs.m_option)
      return true;
    if( p_lhs.m_option == p_rhs.m_option)
      return p_lhs.m_command < p_rhs.m_command;
    return false;
  }

  bool
  operator ==( const TelCommand_t &p_lhs, const TelCommand_t &p_rhs) {
    return
      p_lhs.m_option == p_rhs.m_option &&
      p_lhs.m_command == p_rhs.m_command;
  }

  TelnetClient_t::TelnetClient_t( const InetAddr_t &p_iaddr) :
    TcpClientSocket_t( p_iaddr) {
  }

  ssize_t
  TelnetClient_t::Read( char *p_buf, size_t p_len) {
    int l_nread;
    while( 1) {
      l_nread = TcpClientSocket_t::Read( p_buf, p_len);

      if( l_nread <= 0)
	return l_nread;

      stringstream l_is( p_buf);
      //l_is.str( (char *)buf);

      string l_cmdanswer;
      while( l_nread > 0) {
	if( l_is.peek() == TelCommand_t::IAC) {
	  /* int l_iac = */ l_is.get();
	  int l_command = l_is.get();
	  int l_option = l_is.get();
	  l_cmdanswer += this->ProcessCommand( TelCommand_t( l_command, l_option));
	  l_nread -= 3;
	  continue;
	} else {
	  break;
	}
      }

      if( l_cmdanswer.size() > 0)
	this->Write( l_cmdanswer.c_str(), l_cmdanswer.size());
      if( l_nread != 0) {
      // If there is data to be sent back, send it back
	l_is.read( p_buf, l_nread);
	break;
      }
    }
    return l_nread;
  }

  bool
  TelnetClient_t::RunScript( const TelScript_t &p_Telscript, const string &p_initstr) {
    const int l_bufsize = 1 * 1024;
    char l_buf[ l_bufsize];
    string l_line;
    
    if( p_initstr.size() > 0)
      this->Write( p_initstr.c_str(), p_initstr.size());

    int l_nread;
    TelScript_t::const_iterator l_it = p_Telscript.begin();

    int l_count = 0;
    while( true) {
      l_count++;
      if( l_it == p_Telscript.end())
	break;

      l_nread = this->Read( l_buf, l_bufsize);
      if( l_nread <= 0)
	return false;
      
      const TelActionReaction_t &l_tc = *l_it;
      stringstream l_sstream( string( (char *)l_buf, l_nread));
      string l_line;
      while( ::std::getline( l_sstream, l_line)) {
	if( l_tc.m_expect.size() == 0 || l_line.find( l_tc.m_expect) != string::npos) {
	  l_it++;
	  if( l_tc.m_answer.size() > 0)
	    this->Write( l_tc.m_answer.c_str(), l_tc.m_answer.size());
	  break;
	}
      }
    }
    // Empty buffer
    while( this->Read( l_buf, l_bufsize) > 0);
    return true;
  }

  string
  TelnetClient_t::ProcessCommand( const TelCommand_t &p_cmd) {
    // Answer according to possibilities
    TelOptions_t::const_iterator l_it = m_options.find( p_cmd);
    
    string l_answer;
    if( l_it != m_options.end()) {
      l_answer = TelCommand_t::CommandString( (*l_it).second, p_cmd.m_option);
    } else {
      // PREDEFINED ANSWERS
      if( p_cmd.m_command == TelCommand_t::DO || p_cmd.m_command == TelCommand_t::DONT) {
	l_answer = TelCommand_t::CommandString( TelCommand_t::WONT, p_cmd.m_option);

      } else if( p_cmd.m_command == TelCommand_t::WILL || p_cmd.m_command == TelCommand_t::WONT) {
	l_answer = TelCommand_t::CommandString( TelCommand_t::DONT, p_cmd.m_option);

      }
    }
    return l_answer;
  }

  string
  TelnetClient_t::ProcessCommand( int p_command, int p_option) {
    return this->ProcessCommand( TelCommand_t( p_command, p_option));
  }
}
