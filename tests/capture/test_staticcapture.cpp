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
   Copyright (C) Wallix 2012
   Author(s): Christophe Grosjean

   Unit test to conversion of RDP drawing orders to PNG images
*/

#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestStaticCapture
#include <boost/test/auto_unit_test.hpp>

#define LOGNULL
#include "test_orders.hpp"
#include "transport.hpp"
#include "outfilenametransport.hpp"
#include "image_capture.hpp"
#include "staticcapture.hpp"
#include "RDP/caches/bmpcache.hpp"
#include <png.h>


BOOST_AUTO_TEST_CASE(TestOneRedScreen)
{
    Rect screen_rect(0, 0, 800, 600);
    const int groupid = 0;
    OutFilenameSequenceTransport trans(FilenameGenerator::PATH_FILE_PID_COUNT_EXTENSION, "./", "test", ".png", groupid);

    struct timeval now;
    now.tv_sec = 1350998222;
    now.tv_usec = 0;

    Inifile ini;
    ini.video.png_limit = 3;
    ini.video.png_interval = 20;
    RDPDrawable drawable(800, 600);
    StaticCapture consumer(now, trans, trans.seqgen(), 800, 600, false, ini, drawable.drawable);

    consumer.set_pointer_display();

    bool ignore_frame_in_timeval = false;

    RDPOpaqueRect cmd(Rect(0, 0, 800, 600), RED);
    drawable.draw(cmd, screen_rect);
    consumer.snapshot(now, 10, 10, ignore_frame_in_timeval);
    now.tv_sec++;
    consumer.snapshot(now, 10, 10, ignore_frame_in_timeval);
    now.tv_sec++;
    consumer.snapshot(now, 10, 10, ignore_frame_in_timeval);
    now.tv_sec++;
    consumer.snapshot(now, 10, 10, ignore_frame_in_timeval);
    now.tv_sec++;

    RDPOpaqueRect cmd1(Rect(100, 100, 200, 200), BLUE);
    drawable.draw(cmd1, screen_rect);
    consumer.snapshot(now, 10, 10, ignore_frame_in_timeval);
    now.tv_sec++;
    consumer.snapshot(now, 10, 10, ignore_frame_in_timeval);
    now.tv_sec++;

    BOOST_CHECK_EQUAL(3065, ::filesize(trans.seqgen()->get(0)));
    BOOST_CHECK_EQUAL(3083, ::filesize(trans.seqgen()->get(1)));
    ::unlink(trans.seqgen()->get(0));
    ::unlink(trans.seqgen()->get(1));
}

BOOST_AUTO_TEST_CASE(TestFrameMarker)
{
    Rect screen_rect(0, 0, 800, 600);
    const int groupid = 0;
    OutFilenameSequenceTransport trans(FilenameGenerator::PATH_FILE_PID_COUNT_EXTENSION, "./", "test", ".png", groupid);

    struct timeval now;
    now.tv_sec = 1350998222;
    now.tv_usec = 0;

    Inifile ini;
    ini.video.png_limit = 3;
    ini.video.png_interval = 20;
    RDPDrawable drawable(800, 600);
    StaticCapture consumer(now, trans, trans.seqgen(), 800, 600, false, ini, drawable.drawable);

    consumer.set_pointer_display();

    bool ignore_frame_in_timeval = false;

    RDP::FrameMarker order;
    order.action = RDP::FrameMarker::FrameStart;
    drawable.draw(order);

    RDPOpaqueRect cmd(Rect(0, 0, 800, 600), RED);
    drawable.draw(cmd, screen_rect);

    consumer.snapshot(now, 10, 10, ignore_frame_in_timeval);
    now.tv_sec++;
    consumer.snapshot(now, 10, 10, ignore_frame_in_timeval);
    now.tv_sec++;

    consumer.snapshot(now, 10, 10, ignore_frame_in_timeval);
    now.tv_sec++;

    RDPOpaqueRect cmd1(Rect(50, 50, 80, 60), GREEN);
    drawable.draw(cmd1, screen_rect);

    order.action = RDP::FrameMarker::FrameEnd;
    drawable.draw(order);

    // -> PNG
    consumer.snapshot(now, 10, 10, ignore_frame_in_timeval);
    now.tv_sec++;

    RDPOpaqueRect cmd2(Rect(100, 100, 200, 200), BLUE);
    drawable.draw(cmd2, screen_rect);

    // -> PNG
    consumer.snapshot(now, 10, 10, ignore_frame_in_timeval);
    now.tv_sec++;
    consumer.snapshot(now, 10, 10, ignore_frame_in_timeval);
    now.tv_sec++;

    BOOST_CHECK_EQUAL(3075, ::filesize(trans.seqgen()->get(0)));
    BOOST_CHECK_EQUAL(3108, ::filesize(trans.seqgen()->get(1)));
    ::unlink(trans.seqgen()->get(0));
    ::unlink(trans.seqgen()->get(1));
}

