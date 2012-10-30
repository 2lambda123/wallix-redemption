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
   Author(s): Christophe Grosjean

   Unit test to RDP Orders coder/decoder
   Using lib boost functions for testing
*/


#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestOrderGlyphIndex
#include <boost/test/auto_unit_test.hpp>
#include <algorithm>

#include "RDP/orders/RDPOrdersCommon.hpp"
#include "RDP/orders/RDPOrdersPrimaryGlyphIndex.hpp"

#include "./test_orders.hpp"

BOOST_AUTO_TEST_CASE(TestGlyphIndex)
{
    using namespace RDP;

    {
        BStream stream(1000);
     TODO(" actual data is much more complex  than a text  we should create a specialized object to store  serialize and replay it. This should be done after the RDP layer includes cache management primitives")

        RDPOrderCommon state_common(0, Rect(0, 0, 0, 0));
        RDPGlyphIndex statecmd(0, 0, 0, 0, 0, 0, Rect(), Rect(), RDPBrush(), 0, 0, 0, (uint8_t*)"");
        RDPOrderCommon newcommon(GLYPHINDEX, Rect(5, 0, 800, 600));
        RDPGlyphIndex newcmd(1, 0x20, 1, 4,
                             0x112233,
                             0x445566,
                             Rect(1,2,40,60),
                             Rect(3,4,50,70),
                             RDPBrush(3, 4, 0x03, 0xDD, (uint8_t*)"\1\2\3\4\5\6\7"),
                             5, 6,
                             12, (uint8_t*)"Hello, World");

        newcmd.emit(stream, newcommon, state_common, statecmd);

        BOOST_CHECK_EQUAL((uint8_t)GLYPHINDEX, newcommon.order);
        BOOST_CHECK_EQUAL(Rect(5, 0, 800, 600), newcommon.clip);

        TODO(" DELTA is disabled because it does not works with rdesktop")
        uint8_t datas[] = {
            CHANGE | STANDARD | BOUNDS,
            GLYPHINDEX,
            0xff, 0xff, 0x3f, // fields
            0x1c, 0x05, 0x24, 0x03, 0x57, 0x02, // bounds
            0x01, // font cache_id
            0x20, // flags
            0x01, // charinc
            0x04, // f_op_redundant
            0x33, 0x22, 0x11, // back_color
            0x66, 0x55, 0x44, // fore_color
            0x01, 0x00, 0x02, 0x00, 0x28, 0x00, 0x3d, 0x00, // bk rect
            0x03, 0x00, 0x04, 0x00, 0x34, 0x00, 0x49, 0x00, // op rect
            0x03, 0x04, 0x03, 0xdd, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, // brush
            0x05, 0x00, 0x06, 0x00,// glyph_x, glyph_y
            // 12, "Hello, World"
            0x0c, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64,
        };

        check_datas(stream.p-stream.data, stream.data, sizeof(datas), datas, "Text 1");
        stream.mark_end(); stream.p = stream.data;

        RDPOrderCommon common_cmd = state_common;
        uint8_t control = stream.in_uint8();
        BOOST_CHECK_EQUAL(true, !!(control & STANDARD));
        RDPPrimaryOrderHeader header = common_cmd.receive(stream, control);

        BOOST_CHECK_EQUAL((uint8_t)0x0D, header.control);
        BOOST_CHECK_EQUAL((uint32_t)0x3fffff, header.fields);
        BOOST_CHECK_EQUAL((uint8_t)GLYPHINDEX, common_cmd.order);
        BOOST_CHECK_EQUAL(Rect(5, 0, 800, 600), common_cmd.clip);

        RDPGlyphIndex cmd = statecmd;
        cmd.receive(stream, header);

        if (!(RDPBrush(3, 4, 0x03, 0xDD, (uint8_t*)"\1\2\3\4\5\6\7")
                == cmd.brush)){
            BOOST_CHECK_EQUAL(true, false);
        }
     TODO(" actual data is much more complex than a text  we should create a specialized object to store  serialize and replay it. This should be done after the RDP layer includes cache management primitives")
        check<RDPGlyphIndex>(common_cmd, cmd,
            RDPOrderCommon(GLYPHINDEX, Rect(5, 0, 800, 600)),
            RDPGlyphIndex(1, 0x20, 1, 4,
                          0x112233,
                          0x445566,
                          Rect(1,2,40,60),
                          Rect(3,4,50,70),
                          RDPBrush(3, 4, 0x03, 0xDD, (uint8_t*)"\1\2\3\4\5\6\7"),
                          5, 6,
                          12, (uint8_t*)"Hello, World"),
            "Text 1");
    }


}
