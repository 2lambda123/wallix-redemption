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
   Copyright (C) Wallix 2012
   Author(s): Christophe Grosjean

   Unit test to conversion of RDP drawing orders to PNG images
*/

#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestWrmCapture
#include <boost/test/auto_unit_test.hpp>

#define LOGNULL
//#define LOGPRINT
#include "capture.hpp"

BOOST_AUTO_TEST_CASE(TestSplittedCapture)
{
    Inifile ini;
    const int groupid = 0;
    {
        // Timestamps are applied only when flushing
        struct timeval now;
        now.tv_usec = 0;
        now.tv_sec = 1000;

        Rect scr(0, 0, 800, 600);

        ini.video.frame_interval = 100; // one timestamp every second
        ini.video.break_interval = 3;   // one WRM file every 5 seconds

        ini.video.png_limit = 10; // one snapshot by second
        ini.video.png_interval = 10; // one snapshot by second

        ini.video.capture_wrm = true;
        ini.video.capture_png = true;
        ini.globals.enable_file_encryption.set(false);
        Capture capture(now, scr.cx, scr.cy, "./", "./", "/tmp/", "capture", false, false, NULL, ini);

        bool ignore_frame_in_timeval = false;

        capture.draw(RDPOpaqueRect(scr, GREEN), scr);
        now.tv_sec++;
        capture.snapshot(now, 0, 0, ignore_frame_in_timeval);

        capture.draw(RDPOpaqueRect(Rect(1, 50, 700, 30), BLUE), scr);
        now.tv_sec++;
        capture.snapshot(now, 0, 0, ignore_frame_in_timeval);

        capture.draw(RDPOpaqueRect(Rect(2, 100, 700, 30), WHITE), scr);
        now.tv_sec++;
        capture.snapshot(now, 0, 0, ignore_frame_in_timeval);

        // ------------------------------ BREAKPOINT ------------------------------

        capture.draw(RDPOpaqueRect(Rect(3, 150, 700, 30), RED), scr);
        now.tv_sec++;
        capture.snapshot(now, 0, 0, ignore_frame_in_timeval);

        capture.draw(RDPOpaqueRect(Rect(4, 200, 700, 30), BLACK), scr);
        now.tv_sec++;
        capture.snapshot(now, 0, 0, ignore_frame_in_timeval);

        capture.draw(RDPOpaqueRect(Rect(5, 250, 700, 30), PINK), scr);
        now.tv_sec++;
        capture.snapshot(now, 0, 0, ignore_frame_in_timeval);

        // ------------------------------ BREAKPOINT ------------------------------

        capture.draw(RDPOpaqueRect(Rect(6, 300, 700, 30), WABGREEN), scr);
        now.tv_sec++;
        capture.snapshot(now, 0, 0, ignore_frame_in_timeval);

        capture.flush(); // to close last wrm

        const char * filename;

        filename = capture.png_trans->seqgen()->get(0);
        BOOST_CHECK_EQUAL(3067, ::filesize(filename));
        ::unlink(filename);
        filename = capture.png_trans->seqgen()->get(1);
        BOOST_CHECK_EQUAL(3094, ::filesize(filename));
        ::unlink(filename);
        filename = capture.png_trans->seqgen()->get(2);
        BOOST_CHECK_EQUAL(3107, ::filesize(filename));
        ::unlink(filename);
        filename = capture.png_trans->seqgen()->get(3);
        BOOST_CHECK_EQUAL(3129, ::filesize(filename));
        ::unlink(filename);
        filename = capture.png_trans->seqgen()->get(4);
        BOOST_CHECK_EQUAL(3139, ::filesize(filename));
        ::unlink(filename);
        filename = capture.png_trans->seqgen()->get(5);
        BOOST_CHECK_EQUAL(3165, ::filesize(filename));
        ::unlink(filename);
        filename = capture.png_trans->seqgen()->get(6);
        BOOST_CHECK_EQUAL(3190, ::filesize(filename));
        ::unlink(filename);

        const SequenceGenerator * wrm_seq = capture.seqgen();
        filename = wrm_seq->get(0);
        BOOST_CHECK_EQUAL(1625, ::filesize(filename));
        ::unlink(filename);
        filename = wrm_seq->get(1);
        BOOST_CHECK_EQUAL(3487, ::filesize(filename));
        ::unlink(filename);
        filename = wrm_seq->get(2);
        BOOST_CHECK_EQUAL(3463, ::filesize(filename));
        ::unlink(filename);
        // The destruction of capture object will finalize the metafile content
    }

    {
        FilenameGenerator wrm_seq(FilenameGenerator::PATH_FILE_PID_EXTENSION, "./", "capture", ".mwrm", groupid);
        const char * filename = wrm_seq.get(0);
        BOOST_CHECK_EQUAL(124, ::filesize(filename));
        ::unlink(filename);
    }

    if (ini.globals.enable_file_encryption.get()) {
        FilenameGenerator wrm_seq(FilenameGenerator::PATH_FILE_PID_EXTENSION, "/tmp/", "capture", ".mwrm", groupid);
        const char * filename = wrm_seq.get(0);
        BOOST_CHECK_EQUAL(32, ::filesize(filename));
        ::unlink(filename);
    }
}
