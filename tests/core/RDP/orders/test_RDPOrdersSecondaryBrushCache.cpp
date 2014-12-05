/*
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   Product name: redemption, a FLOSS RDP proxy
   Copyright (C) Wallix 2010
   Author(s): Christophe Grosjean

   Unit test to RDP Orders coder/decoder
   Using lib boost functions for testing
*/

#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestOrderColCache
#include <boost/test/auto_unit_test.hpp>

#include "RDP/orders/RDPOrdersSecondaryBrushCache.hpp"

#include "test_orders.hpp"

BOOST_AUTO_TEST_CASE(TestBrushCache1BPP)
{
    using namespace RDP;

    {
        BStream stream(65536);

        uint8_t pattern[] = {
            0, 0, 0, 0, 0, 0, 0, 0,
        };

        RDPBrushCache newcmd(0, BMF_1BPP, 8, 8, 0, 8, pattern);

        newcmd.emit(stream);

        uint8_t datas[] = {
            STANDARD | SECONDARY,       // control = 0x03
            0x07, 0x00,     // length (7 means 13 + 7 = 20)
            0x00, 0x00,     // extraFlags
            TS_CACHE_BRUSH, // type

            0x00, // cacheIndex
            BMF_1BPP,
            0x08, // cx
            0x08, // cy
            0x00, // Style (unused should be 0)
            0x08, // iBytes
            // data
            0, 0, 0, 0, 0, 0, 0, 0,
        };

        // BMF_1BPP && iBytes 8 : top-down back/front color bitmap
        // BMF_8BPP && iBytes 20 : compression (16 bytes for 64 x 4 colors pixels, + 4 bytes color definitions)
        // BMF_16BPP && iBytes 24 : compression (16 bytes for 64 x 4 colors pixels, + 8 bytes color definitions)
        // BMF_24BPP && iBytes 28 : compression (16 bytes for 64 x 4 colors pixels, + 12 bytes color definitions)
        // Other iBytes values : uncompressed bitmap data

        check_datas(stream.p-stream.get_data(), stream.get_data(), sizeof(datas), datas, "Brush Cache 1BPP");
        stream.mark_end(); stream.p = stream.get_data();

        uint8_t control = stream.in_uint8();
        BOOST_CHECK_EQUAL(true, !!(control & (STANDARD|SECONDARY)));
        RDPSecondaryOrderHeader header(stream);

        RDPBrushCache cmd(0, BMF_1BPP, 8, 8, 0, 8, pattern);
        cmd.receive(stream, control, header);

        check<RDPBrushCache>(cmd, newcmd, "Brush Cache 1BPP");
    }
}
