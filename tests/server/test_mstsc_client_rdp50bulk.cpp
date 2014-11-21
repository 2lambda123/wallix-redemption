/*
    This program is free software; you can redistribute it and/or modify it
     under the terms of the GNU General Public License as published by the
     Free Software Foundation; either version 2 of the License, or (at your
     option) any later version.

    This program is distributed in the hope that it will be useful, but
     WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
     Public License for more details.

    You should have received a copy of the GNU General Public License along
     with this program; if not, write to the Free Software Foundation, Inc.,
     675 Mass Ave, Cambridge, MA 02139, USA.

    Product name: redemption, a FLOSS RDP proxy
    Copyright (C) Wallix 2013
    Author(s): Christophe Grosjean, Javier Caverni, Raphael Zhou
    Based on xrdp Copyright (C) Jay Sorg 2004-2010

    Unit test to check front-end behavior stays identical
    when connecting from mstsc (mocked up)
*/

#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestFrontMstscClientRDP50Bulk

#include <boost/test/auto_unit_test.hpp>

#include <errno.h>
#include <algorithm>
#include <sys/un.h>

#undef SHARE_PATH
#define SHARE_PATH "./tests/fixtures"

#undef DEFAULT_FONT_NAME
#define DEFAULT_FONT_NAME "sans-10.fv1"

#define LOGNULL
//#define LOGPRINT
#include "log.hpp"

#include "listen.hpp"
#include "session.hpp"
#include "test_transport.hpp"

BOOST_AUTO_TEST_CASE(TestIncomingConnection)
{
    // This server only support one incoming connection before closing listener
//     class ServerOnce : public Server {
//     public:
//         int  sck;
//         char ip_source[256];
//
//         ServerOnce() : sck(0) {
//             ip_source[0] = 0;
//         }
//
//         virtual Server_status start(int incoming_sck)
//         {
//             union {
//                 struct sockaddr s;
//                 struct sockaddr_storage ss;
//                 struct sockaddr_in s4;
//                 struct sockaddr_in6 s6;
//             } u;
//             unsigned int sin_size = sizeof(u);
//             ::memset(&u, 0, sin_size);
//             this->sck = ::accept(incoming_sck, &u.s, &sin_size);
//             strcpy(ip_source, ::inet_ntoa(u.s4.sin_addr));
//             LOG(LOG_INFO, "Incoming socket to %d (ip=%s)\n", sck, ip_source);
//             return START_WANT_STOP;
//         }
//     } one_shot_server;
//     Listen listener(one_shot_server, 0, 3389, true, 5); // 25 seconds to connect, or timeout
//     listener.run();

    Inifile ini;
    ini.debug.front = 511;
    ini.debug.primary_orders = 7;

//     int nodelay = 1;
//     if (-1 == setsockopt( one_shot_server.sck, IPPROTO_TCP, TCP_NODELAY
//                         , (char *)&nodelay, sizeof(nodelay))) {
//         LOG(LOG_INFO, "Failed to set socket TCP_NODELAY option on client socket");
//     }
//     SocketTransport front_trans( "RDP Client", one_shot_server.sck, "0.0.0.0", 0
//                                , ini.debug.front, 0);
//     wait_obj front_event(&front_trans);

    LCGRandom gen(0);

    #include "fixtures/trace_mstsc_client_rdp50bulk.hpp"

    uint32_t     verbose = 511;
    const char * name    = "Test Front Transport";
    TestTransport front_trans(name, indata, sizeof(indata), outdata, sizeof(outdata),
        verbose);

    ini.client.tls_support         = true;
    ini.client.tls_fallback_legacy = false;
    ini.client.rdp_compression     = 2;     // RDP 5.0 bulk compression

    const bool fastpath_support = true;
    const bool mem3blt_support  = false;

    Front front( front_trans, SHARE_PATH "/" DEFAULT_FONT_NAME, gen, ini
               , fastpath_support, mem3blt_support);
    null_mod no_mod(front);

    while (front.up_and_running == 0) {
        front.incoming(no_mod);
    }

    // LOG(LOG_INFO, "hostname=%s",front.client_info.hostname);
    // BOOST_CHECK_EQUAL(0, memcmp(front.client_info.hostname, "MATHIEU-LAPTOP\0\0", 16));

    BOOST_CHECK_EQUAL(1, front.up_and_running);
    TestCardMod mod(front, front.client_info.width, front.client_info.height);
    mod.draw_event(time(NULL));

//     sleep(5);
//     shutdown(one_shot_server.sck, 2);
//     close(one_shot_server.sck);

//     LOG(LOG_INFO, "Listener closed\n");
//     LOG( LOG_INFO, "Incoming socket %d (ip=%s)\n", one_shot_server.sck
//        , one_shot_server.ip_source);
}
