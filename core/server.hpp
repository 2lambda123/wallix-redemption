/*
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   Product name: redemption, a FLOSS RDP proxy
   Copyright (C) Wallix 2010
   Author(s): Christophe Grosjean, Javier Caverni
   Based on xrdp Copyright (C) Jay Sorg 2004-2010

   Server abstraction

*/

#if !defined(CORE_SERVER_HPP)
#define CORE_SERVER_HPP

class Server
{
    public:
    enum Server_status 
    { START_OK        // Server started, ready to wait next incoming connecion
    , START_FAILED    // Server failed to start, but listener should wait for next incoming connection
    , START_WANT_STOP // Child process terminating or main process willing server to stop
    };
    
    virtual Server_status start(int sck) = 0;
};

#endif
