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
   Copyright (C) Wallix 2013
   Author(s): Christophe Grosjean

*/

#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestFileToGraphic
#include <boost/test/auto_unit_test.hpp>

#define LOGNULL
//#define LOGPRINT

#include "outfilenametransport.hpp"
#include "infiletransport.hpp"
#include "staticcapture.hpp"
#include "nativecapture.hpp"
#include "FileToGraphic.hpp"

BOOST_AUTO_TEST_CASE(TestSample0WRM)
{
    const char * input_filename = "./tests/fixtures/sample0.wrm";
    char path[1024];
    size_t len = strlen(input_filename);
    memcpy(path, input_filename, len);
    path[len] = 0;

    int fd = ::open(path, O_RDONLY);
    if (fd == -1){
        LOG(LOG_INFO, "open '%s' failed with error : %s", path, strerror(errno));
        BOOST_CHECK(false);
        return;
    }

    InFileTransport in_wrm_trans(fd);
    timeval begin_capture;
    begin_capture.tv_sec = 0; begin_capture.tv_usec = 0;
    timeval end_capture;
    end_capture.tv_sec = 0; end_capture.tv_usec = 0;
    FileToGraphic player(&in_wrm_trans, begin_capture, end_capture, false, 0);

    Inifile ini;
    ini.debug.primary_orders = 0;
    ini.debug.secondary_orders = 0;

    const int groupid = 0;
    OutFilenameTransport out_png_trans(SQF_PATH_FILE_PID_COUNT_EXTENSION, "./", "first", ".png", groupid);
    RDPDrawable drawable1(player.screen_rect.cx, player.screen_rect.cy);
    ImageCapture png_recorder(out_png_trans, player.screen_rect.cx, player.screen_rect.cy, drawable1.drawable);

    png_recorder.update_config(ini);
    player.add_consumer(&drawable1);

    OutFilenameTransport out_wrm_trans(SQF_PATH_FILE_PID_COUNT_EXTENSION, "./", "first", ".wrm", groupid);
    ini.video.frame_interval = 10;
    ini.video.break_interval = 20;

    BmpCache bmp_cache(
        BmpCache::Recorder,
        player.bmp_cache->bpp,
        player.bmp_cache->number_of_cache,
        player.bmp_cache->use_waiting_list,
        player.bmp_cache->cache_entries[0],
        player.bmp_cache->cache_size[0],
        player.bmp_cache->cache_persistent[0],
        player.bmp_cache->cache_entries[1],
        player.bmp_cache->cache_size[1],
        player.bmp_cache->cache_persistent[1],
        player.bmp_cache->cache_entries[2],
        player.bmp_cache->cache_size[2],
        player.bmp_cache->cache_persistent[2],
        player.bmp_cache->cache_entries[3],
        player.bmp_cache->cache_size[3],
        player.bmp_cache->cache_persistent[3],
        player.bmp_cache->cache_entries[4],
        player.bmp_cache->cache_size[4],
        player.bmp_cache->cache_persistent[4]
        );

    RDPDrawable drawable(player.screen_rect.cx, player.screen_rect.cy);
    NativeCapture wrm_recorder(
        player.record_now,
        out_wrm_trans,
        player.screen_rect.cx,
        player.screen_rect.cy,
        bmp_cache, drawable, ini);

    wrm_recorder.update_config(ini);
    player.add_consumer(&wrm_recorder);

    BOOST_CHECK_EQUAL((unsigned)1352304810, (unsigned)player.record_now.tv_sec);
    player.play();

    png_recorder.flush();
    BOOST_CHECK_EQUAL((unsigned)1352304870, (unsigned)player.record_now.tv_sec);

    wrm_recorder.flush();
    BOOST_CHECK_EQUAL((unsigned)21280, (unsigned)sq_outfilename_filesize(&(out_png_trans.seq), 0));
    sq_outfilename_unlink(&(out_png_trans.seq), 0);

    BOOST_CHECK_EQUAL(static_cast<unsigned>(490438),  static_cast<unsigned>(sq_outfilename_filesize(&(out_wrm_trans.seq), 0)));
    sq_outfilename_unlink(&(out_wrm_trans.seq), 0);
    BOOST_CHECK_EQUAL(static_cast<unsigned>(1247494), static_cast<unsigned>(sq_outfilename_filesize(&(out_wrm_trans.seq), 1)));
    sq_outfilename_unlink(&(out_wrm_trans.seq), 1);
    BOOST_CHECK_EQUAL(static_cast<unsigned>(363530),  static_cast<unsigned>(sq_outfilename_filesize(&(out_wrm_trans.seq), 2)));
    sq_outfilename_unlink(&(out_wrm_trans.seq), 2);
}

//BOOST_AUTO_TEST_CASE(TestSecondPart)
//{
//    const char * input_filename = "./tests/fixtures/sample1.wrm";
//    char path[1024];
//    size_t len = strlen(input_filename);
//    memcpy(path, input_filename, len);
//    path[len] = 0;

//    int fd = ::open(path, O_RDONLY);
//    if (fd == -1){
//        LOG(LOG_INFO, "open '%s' failed with error : %s", path, strerror(errno));
//        BOOST_CHECK(false);
//        return;
//    }

//    InFileTransport in_wrm_trans(fd);
//    timeval begin_capture;
//    begin_capture.tv_sec = 0; begin_capture.tv_usec = 0;
//    timeval end_capture;
//    end_capture.tv_sec = 0; end_capture.tv_usec = 0;
//    FileToGraphic player(&in_wrm_trans, begin_capture, end_capture, false, 0);

//    Inifile ini;
//    ini.debug.primary_orders = 0;
//    ini.debug.secondary_orders = 0;

//    const int groupid = 0;
//    OutFilenameTransport out_png_trans(SQF_PATH_FILE_PID_COUNT_EXTENSION, "./", "second_part", ".png", groupid);
//    RDPDrawable drawable1(player.screen_rect.cx, player.screen_rect.cy);
//    ImageCapture png_recorder(out_png_trans, player.screen_rect.cx, player.screen_rect.cy, drawable1.drawable);

//    png_recorder.update_config(ini);
//    player.add_consumer(&drawable1);

//    OutFilenameTransport out_wrm_trans(SQF_PATH_FILE_PID_COUNT_EXTENSION, "./", "second_part", ".wrm", groupid);
//    ini.video.frame_interval = 10;
//    ini.video.break_interval = 20;

//    BmpCache bmp_cache(
//        BmpCache::Recorder,
//        player.bmp_cache->bpp,
//        player.bmp_cache->number_of_cache,
//        player.bmp_cache->use_waiting_list,
//        player.bmp_cache->cache_0_entries,
//        player.bmp_cache->cache_0_size,
//        player.bmp_cache->cache_0_persistent,
//        player.bmp_cache->cache_1_entries,
//        player.bmp_cache->cache_1_size,
//        player.bmp_cache->cache_1_persistent,
//        player.bmp_cache->cache_2_entries,
//        player.bmp_cache->cache_2_size,
//        player.bmp_cache->cache_2_persistent,
//        player.bmp_cache->cache_3_entries,
//        player.bmp_cache->cache_3_size,
//        player.bmp_cache->cache_3_persistent,
//        player.bmp_cache->cache_4_entries,
//        player.bmp_cache->cache_4_size,
//        player.bmp_cache->cache_4_persistent);

//    RDPDrawable drawable(player.screen_rect.cx, player.screen_rect.cy);
//    NativeCapture wrm_recorder(
//        player.record_now,
//        out_wrm_trans,
//        player.screen_rect.cx,
//        player.screen_rect.cy,
//        bmp_cache, drawable, ini);

//    wrm_recorder.update_config(ini);
//    player.add_consumer(&wrm_recorder);

//    BOOST_CHECK_EQUAL((unsigned)1352304870, (unsigned)player.record_now.tv_sec);
//    player.play();
//    BOOST_CHECK_EQUAL((unsigned)1352304928, (unsigned)player.record_now.tv_sec);

//    png_recorder.flush();

//    TODO("check RGB/BGR: fixed test replacing 47483 with 47553")
//    BOOST_CHECK_EQUAL((unsigned)47553, (unsigned)sq_outfilename_filesize(&(out_png_trans.seq), 0));
//    sq_outfilename_unlink(&(out_png_trans.seq), 0);

//    wrm_recorder.flush();
//    BOOST_CHECK_EQUAL((unsigned)74803, (unsigned)sq_outfilename_filesize(&(out_wrm_trans.seq), 0));
//    sq_outfilename_unlink(&(out_wrm_trans.seq), 0);
//    // Mem3Blt save state = 34 bytes
//    BOOST_CHECK_EQUAL(static_cast<unsigned>(273774) + 34, (unsigned)sq_outfilename_filesize(&(out_wrm_trans.seq), 1));
//    sq_outfilename_unlink(&(out_wrm_trans.seq), 1);
//    // Mem3Blt save state = 34 bytes
//    BOOST_CHECK_EQUAL(static_cast<unsigned>(185108) + 34, (unsigned)sq_outfilename_filesize(&(out_wrm_trans.seq), 2));
//    sq_outfilename_unlink(&(out_wrm_trans.seq), 2);
//}

