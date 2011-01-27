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

#ifndef __DSOCK_IO_HPP__
#define __DSOCK_IO_HPP__

#include <unistd.h>
#include <sys/types.h>

#include <map>

namespace dsock {

  class io_t {
  public:
    const static io_t zerio;
  protected:
    typedef std::map<int, int> iomap_t;
    static iomap_t sm_iomap;

  protected:
    int m_handle;

  public:
    virtual const char *className( void) const { return "io_t"; };

    io_t( int fd = -1);
    io_t( const io_t &io);
    virtual ~io_t();

  protected:
    virtual void open( int fd, bool doclose = true);
    virtual void close( void);

  public:
    virtual io_t &operator =( const io_t &io);
    virtual int operator !(void) const;
    virtual int operator ==( const io_t &io) const;
    virtual int operator !=( const io_t &io) const;
    virtual int operator <( const io_t &io) const;
    virtual int operator >( const io_t &io) const;
    virtual int operator <=( const io_t &io) const;
    virtual int operator >=( const io_t &io) const;

    virtual int handle( void) const;

    virtual int read( void *buf, int len) const;
    virtual int write( const void *buf, int len) const;
  };

#if 0
  struct ioequal_t {
    bool operator ()(const io_t &io1, const io_t &io2) const {
      return io1 == io2;
    }
  };
#endif

}
#if 0
__STL_TEMPLATE_NULL struct hash<dsock::io_t> {
  size_t operator()(dsock::io_t &io) const { return io.handle(); }
};
#endif

#endif
