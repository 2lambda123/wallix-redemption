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
#define BOOST_TEST_MODULE TestOrderLineTo
#include <boost/test/auto_unit_test.hpp>
#include <algorithm>

#include "RDP/orders/RDPOrdersCommon.hpp"
#include "RDP/orders/RDPOrdersPrimaryHeader.hpp"
#include "RDP/orders/RDPOrdersPrimaryLineTo.hpp"

#include "./test_orders.hpp"

BOOST_AUTO_TEST_CASE(TestLineTo)
{
    using namespace RDP;


    {
        Stream stream(1000);

        RDPOrderCommon state_common(0, Rect(0, 0, 0, 0));
        RDPLineTo state_lineto(0, 0, 0, 0, 0, 0, 0, RDPPen(0, 0, 0));
        RDPOrderCommon newcommon(LINE, Rect(10, 20, 30, 40));

        RDPLineTo(1, 0, 10, 40, 60, 0x102030, 0xFF, RDPPen(0, 1, 0x112233)
                  ).emit(stream, newcommon, state_common, state_lineto);


        BOOST_CHECK_EQUAL((uint8_t)LINE, newcommon.order);
        BOOST_CHECK_EQUAL(Rect(10, 20, 30, 40), newcommon.clip);

        uint8_t datas[] = {
            CHANGE | STANDARD | BOUNDS | DELTA,
            LINE,
            0x7D, 0x03,
            0xF0, 0x0A, 0x14, 0x28, 0x3C,
            0x01, 00, // back_mode
            0x0a, // starty = +10,
            0x28, // endx = +40
            0x3C, // endy = +60
            0x30, 0x20, 0x10, // back_color
            0xff, // rop2
            01,              // pen width
            0x33, 0x22, 0x11 // pen color
        };
        check_datas(stream.p-stream.data, stream.data, sizeof(datas), datas, "LineTo 1");

        stream.end = stream.p; stream.p = stream.data;

        RDPOrderCommon common_cmd = state_common;
        uint8_t control = stream.in_uint8();
        BOOST_CHECK_EQUAL(true, !!(control & STANDARD));
        RDPPrimaryOrderHeader header = common_cmd.receive(stream, control);

        BOOST_CHECK_EQUAL((uint8_t)0x1D, header.control);
        BOOST_CHECK_EQUAL((uint32_t)0x37D, header.fields);
        BOOST_CHECK_EQUAL((uint8_t)LINE, common_cmd.order);
        BOOST_CHECK_EQUAL(Rect(10, 20, 30, 40), common_cmd.clip);

        RDPLineTo cmd = state_lineto;
        cmd.receive(stream, header);

        check<RDPLineTo>(common_cmd, cmd,
            RDPOrderCommon(LINE, Rect(10, 20, 30, 40)),
            RDPLineTo(1, 0, 10, 40, 60, 0x102030, 0xFF, RDPPen(0, 1, 0x112233)),
            "LineTo 1");
    }

}
