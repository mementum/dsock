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

#include <sstream>
#include <fstream>
using namespace std;

#include <dsock/ftpclient.hpp>

namespace dsock {

  bool
  FtpClient_t::SendVerb( const string &p_verb, const string &p_param,
			 ssize_t &p_retcode) {

    string l_verb( p_verb);

    if( p_param.size() != 0)
      l_verb += " " + p_param;

    l_verb += "\r\n";

    p_retcode = this->Write( l_verb.c_str(), l_verb.size());

    if( p_retcode == SOCKET_ERROR)
      return false;

    return true;
  }


  bool
  FtpClient_t::SendVerb( const string &p_verb, const vector<string> &p_params,
			 ssize_t &p_retcode) {

    string l_param;
    if( p_params.size() != 0) {
      /* if( p_params.begin() != p_params.end()) */

      vector<string>::const_iterator l_it = p_params.begin();

      l_param = *l_it;
      l_it++;

      while( l_it != p_params.end()) {
	l_param += "," + (*l_it);
	l_it++;
      }
    }

    return this->SendVerb( p_verb, l_param, p_retcode);
  }
  
  bool
  FtpClient_t::WaitResponse( ssize_t &p_retcode) {
    stringstream l_sstream;
    return this->WaitResponse( l_sstream, p_retcode);
  }

  bool
  FtpClient_t::WaitResponse( string &p_str, ssize_t &p_retcode) {
    stringstream l_sstream;
    bool l_retval = this->WaitResponse( l_sstream, p_retcode);
    p_str = l_sstream.str();
    return l_retval;
  }

  bool
  FtpClient_t::WaitResponse( ostream &p_os, ssize_t &p_retcode) {

    while( true) {
      string l_answer;
    
      p_retcode = this->ReadLine( l_answer);

      if( p_retcode == SOCKET_ERROR)
	return false;

      p_os << l_answer << endl;

      FtpResponseLine_t l_respline( l_answer);
      if( l_respline.IsFinal()) {
	p_retcode = l_respline.m_code;
	return true;
      }
    }
    return false;
  }

  bool
  FtpClient_t::SendAndWait( const string &p_verb, const string &p_param,
			    ssize_t &p_retcode) {

    stringstream l_sstream;
    return this->SendAndWait( p_verb, p_param, l_sstream, p_retcode);
  }

  bool
  FtpClient_t::SendAndWait( const string &p_verb, const string &p_param,
			    ostream &p_os, ssize_t &p_retcode) {

    if( this->SendVerb( p_verb, p_param, p_retcode) == false)
      return false;

    while( true) {

      if( this->WaitResponse( p_os, p_retcode) == false)
	return false;

      if( p_retcode < 200)
	continue;
      else if( p_retcode >= 200 && p_retcode < 400)
	return true;
      else if( p_retcode >= 400)
	return false;
    }

    return false;
  }

  bool
  FtpClient_t::User( const string &p_user, ssize_t &p_retcode) {

    return this->SendAndWait( "USER", p_user, p_retcode);
  }

  bool
  FtpClient_t::Pass( const string &p_pass, ssize_t &p_retcode) {

    return this->SendAndWait( "PASS", p_pass, p_retcode);
  }

  bool
  FtpClient_t::Stor( const string &p_filename, const string &p_filedest,
		     TcpClientSocket_t &p_dataconn,
		     ssize_t &p_retcode) {

#if 0
    ifstream l_is( p_filename.c_str());
    if( l_is.is_open() == false) {
      p_retcode = -1;
      return false;
    }
#else
    FILE *l_is = ::fopen( p_filename.c_str(), "rb");
    if( l_is == NULL) {
      p_retcode = -1;
      return false;
    }
#endif

    string::size_type l_pos1 = p_filename.find_last_of( "/\\");

    // Do a Basename for the ftp server
    string l_filename;
    if( l_pos1 != string::npos)
      l_filename = p_filename.substr( l_pos1 + 1);
    else
      l_filename = p_filename;

    return this->Stor( l_filename, p_filedest, l_is, p_dataconn, p_retcode);
  }

#if 0
  bool
  FtpClient_t::Stor( const string &p_filename, const string &p_filedest,
		     istream &p_is, TcpClientSocket_t &p_dataconn,
		     ssize_t &p_retcode) {
  }
#else
  bool
  FtpClient_t::Stor( const string &p_filename, const string &p_filedest,
		     FILE *p_is, TcpClientSocket_t &p_dataconn,
		     ssize_t &p_retcode) {
#endif


    if( this->SendVerb( "STOR", p_filename + " " + p_filedest, p_retcode) == false)
      return false;

    while( true) {

      if( this->WaitResponse( p_retcode) == false)
	return false;

      // This should be 150 if transfer will start
      if( p_retcode < 200)
	break;

      // Anything else is regarded as an error
      if( p_retcode >= 200)
	return false;
    }

    // Binary connection created
    // Start Writing to the connection
    const size_t C_BUFSIZE = 1024;
    char l_buf[ C_BUFSIZE];
    size_t l_nread;
#if 0
    do {
      p_is.read( l_buf, C_BUFSIZE);
      l_nread = p_is.gcount();

      if( l_nread > 0)
	p_dataconn.Write( l_buf, l_nread);

    } while( p_is.good() == true);
    // The Cppreference says
    // if( p_is.bad() == true || p_is.eof() == false) {
    // My logic says ... if an error happened and it's not eof ...
    // The page logic says ... if error or not-eof ... something weird happened
    if( p_is.bad() == true && p_is.eof() == false) {
      p_retcode = -1;
      return false;
    }

#else
  while( true) {
    l_nread = ::fread( l_buf, 1, C_BUFSIZE, p_is);
    p_dataconn.Write( l_buf, l_nread);
    ::_sleep( 10);
    if( l_nread < C_BUFSIZE)
      break;
  }
  if( ::ferror( p_is) != 0) {
    p_retcode = -1;
    return false;
  }
  ::fclose( p_is);
  p_dataconn.Close();
#endif

    while( true) {
      if( this->WaitResponse( p_retcode) == false)
	return false;

      if( p_retcode < 200)
	continue;
      else if( p_retcode >= 200 && p_retcode < 400)
	return true;
      else if( p_retcode >= 400)
	return false;
      
    }
    return false;
  }

  bool
  FtpClient_t::Port( const IpPort_t &p_ipport, TcpClientSocket_t &p_dataconn,
		     ssize_t &p_retcode) {

    return this->Port( InetAddr_t( this->GetLocalIpAddr(), p_ipport),
		       p_dataconn, p_retcode);
  }

  bool
  FtpClient_t::Port( const InetAddr_t &p_taddr, TcpClientSocket_t &p_dataconn,
		     ssize_t &p_retcode) {

    TcpServerSocket_t l_portsocket( p_taddr);
    const InetAddr_t &l_addr = l_portsocket.GetLocalAddr();

    stringstream l_stream;
    l_stream
      << l_addr.GetIpAddrByte( 3) << ","
      << l_addr.GetIpAddrByte( 2) << ","
      << l_addr.GetIpAddrByte( 1) << ","
      << l_addr.GetIpAddrByte( 0) << ","
      << l_addr.GetIpPortByte( 1) << ","
      << l_addr.GetIpPortByte( 0);

    if( this->SendAndWait( "PORT", l_stream.str(), p_retcode) == false)
      return false;

    p_dataconn = l_portsocket.Accept();

    if( p_dataconn == Socket_t::VoidSocket)
      return false;

    return true;
  }

  bool
  FtpClient_t::Pasv( TcpClientSocket_t &p_dataconn, ssize_t &p_retcode) {

    stringstream l_sstream;
    if( this->SendAndWait( "PASV", "", l_sstream, p_retcode) == false)
      return false;

    string l_line;
    FtpResponseLine_t l_respline;
    while( ::std::getline( l_sstream, l_line)) {
      l_respline.Parse( l_line);
      if( l_respline.IsFinal() == true)
	break;
    }

    // l_line should have the last line in the answer
    string::size_type l_pos = l_respline.m_msg.find_first_of( "0123456789");
    if( l_pos == string::npos)
      return false;

    // Do a simple parse with stream
    stringstream l_pasvstream( l_respline.m_msg.substr( l_pos));

    size_t l_ipbyte[4];
    size_t l_portbyte[2];

    l_pasvstream >> l_ipbyte[ 3];
    l_pasvstream.get(); // ","
    l_pasvstream >> l_ipbyte[ 2];
    l_pasvstream.get(); // ","
    l_pasvstream >> l_ipbyte[ 1];
    l_pasvstream.get(); // ","
    l_pasvstream >> l_ipbyte[ 0];
    l_pasvstream.get(); // ","
    l_pasvstream >> l_portbyte[ 1];
    l_pasvstream.get(); // ","
    l_pasvstream >> l_portbyte[ 0];

    stringstream l_ipstream;
    l_ipstream
      << l_ipbyte[ 3] << "." << l_ipbyte[ 2]<< "."
      << l_ipbyte[ 1] << "." << l_ipbyte[ 0];

    stringstream l_portstream;
    l_portstream << ( l_portbyte[ 1]*256 + l_portbyte[ 0]);

    p_dataconn.Reconnect( InetAddr_t( l_ipstream.str(), l_portstream.str()));
    //p_dataconn = TcpClientSocket_t( InetAddr_t( l_ipstream.str(), l_portstream.str()));

    if( p_dataconn == Socket_t::VoidSocket)
      return false;

    return true;
  }

  bool FtpClient_t::Type( const string &p_type,
			  ssize_t &p_retcode) {

    return this->SendAndWait( "TYPE", p_type, p_retcode);
  }

  bool
  FtpClient_t::TypeBin( ssize_t &p_retcode) {

    return this->Type( "I", p_retcode);
  }

  bool
  FtpClient_t::TypeAscii( ssize_t &p_retcode) {

    return this->Type( "A", p_retcode);
  }

  bool
  FtpClient_t::Quit( ssize_t &p_retcode) {
    return this->SendAndWait( "QUIT", "", p_retcode);
  }

#if 0
  bool
  FtpClient_t::Quote( const string &p_quote,
		      ssize_t &p_retcode) {
  }

  bool
  FtpClient_t::Quote( const vector<string> &p_quote,
		      ssize_t &p_retcode) {
  }

  bool
  FtpClient_t::Quote( const string &p_quoteverb, const vector<string> &p_quoteparams,
		      ssize_t &p_retcode) {

  }

  bool
  FtpClient_t::Quote( const string &p_quoteverb, const string &p_quoteparam,
		      ssize_t &p_retcode) {

  }
#endif

  ////////////////////////////////////////////////////////////
  // FtpClientAdam2_t
  ////////////////////////////////////////////////////////////

  FtpClientAdam2_t::FtpClientAdam2_t( const InetAddr_t &p_taddr) :
    FtpClient_t( p_taddr) {
    
#if 0
    ssize_t l_retcode;
    SocketErr_t l_socketerr;

    // Wait initial answer
    if( this->WaitResponse( l_retcode) == false)
      throw( l_socketerr);

    if( this->User( "adam2", l_retcode) == false)
      throw( l_socketerr);
    ::_sleep( 100);
    if( this->Pass( "adam2", l_retcode) == false)
      throw( l_socketerr);
#endif
    // Do nothing else
  }

  bool
  FtpClientAdam2_t::SetEnv( const string &p_var, const string &p_val,
			    ssize_t &p_retcode) {
    
    string l_fullval( p_var + "," + p_val);
    return this->SendAndWait( "SETENV", l_fullval, p_retcode);
  }

  bool
  FtpClientAdam2_t::GetEnv( const string &p_var, string &p_val,
			    ssize_t &p_retcode) {
    return false;
  }

  bool
  FtpClientAdam2_t::Media( const string &p_media,
			   ssize_t &p_retcode) {

    return this->SendAndWait( "MEDIA", p_media, p_retcode);
  }

  bool
  FtpClientAdam2_t::MediaFlash( ssize_t &p_retcode) {
    return this->Media( "FLSH", p_retcode);
  }

  bool
  FtpClientAdam2_t::MediaSdram( ssize_t &p_retcode) {
    return this->Media( "SDRM", p_retcode);
  }

  bool
  FtpClientAdam2_t::Reboot( ssize_t &p_retcode) {
    return this->SendAndWait( "REBOOT", "", p_retcode);
  }

}
