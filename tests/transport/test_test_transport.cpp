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
#define BOOST_TEST_MODULE TestTestTransport
#include "system/redemption_unit_tests.hpp"

#define LOGNULL
//#define LOGPRINT

#include "transport/test_transport.hpp"
#include "core/error.hpp"

BOOST_AUTO_TEST_CASE(TestGeneratorTransport)
{
    // test we can read from a GeneratorTransport;
    GeneratorTransport gt("We read what we provide!", 24);
    char buffer[128] = {};
    char * p = buffer;
    gt.recv(&p, 0);
    BOOST_CHECK_EQUAL(p-buffer, 0);

    p = buffer;
    gt.recv(&p, 1);
    BOOST_CHECK_EQUAL(p-buffer, 1);
    BOOST_CHECK_EQUAL(buffer[0], 'W');
    BOOST_CHECK_EQUAL(buffer[1], 0); // unchanged, not put by GeneratorTransport

    p = buffer;
    gt.recv(&p, 2);
    BOOST_CHECK_EQUAL(p-buffer, 2);
    BOOST_CHECK_EQUAL(buffer[0], 'e');
    BOOST_CHECK_EQUAL(buffer[1], ' ');
    BOOST_CHECK_EQUAL(buffer[2], 0); // unchanged, not put by GeneratorTransport

    p = buffer;
    gt.recv(&p, 9);
    BOOST_CHECK_EQUAL(p-buffer, 9);
    BOOST_CHECK_EQUAL(0, strncmp(buffer, "read what", 9));

    p = buffer;
    gt.recv(&p, 12);
    BOOST_CHECK_EQUAL(p-buffer, 12);
    BOOST_CHECK_EQUAL(0, strncmp(buffer, " we provide!", 12));

    p = buffer;
    gt.recv(&p, 0);
    BOOST_CHECK_EQUAL(p-buffer, 0);

    p = buffer;
    try {
        gt.recv(&p, 1);
        BOOST_CHECK_EQUAL(true, false);
    } catch (Error & e) {
        BOOST_CHECK_EQUAL(p-buffer, 0);
        BOOST_CHECK_EQUAL(e.id, static_cast<int>(ERR_TRANSPORT_NO_MORE_DATA));
    }
}

BOOST_AUTO_TEST_CASE(TestGeneratorTransport2)
{
    // test we can read from a GeneratorTransport;
    GeneratorTransport gt("We read what we provide!", 24);
    char buffer[128] = {};
    char * p = buffer;
    gt.recv(&p, 0);
    BOOST_CHECK_EQUAL(p-buffer, 0);

    p = buffer;
    gt.recv(&p, 1);
    BOOST_CHECK_EQUAL(p-buffer, 1);
    BOOST_CHECK_EQUAL(buffer[0], 'W');
    BOOST_CHECK_EQUAL(buffer[1], 0); // unchanged, not put by GeneratorTransport

    p = buffer;
    gt.recv(&p, 2);
    BOOST_CHECK_EQUAL(p-buffer, 2);
    BOOST_CHECK_EQUAL(buffer[0], 'e');
    BOOST_CHECK_EQUAL(buffer[1], ' ');
    BOOST_CHECK_EQUAL(buffer[2], 0); // unchanged, not put by GeneratorTransport

    p = buffer;
    gt.recv(&p, 9);
    BOOST_CHECK_EQUAL(p-buffer, 9);
    BOOST_CHECK_EQUAL(0, strncmp(buffer, "read what", 9));

    p = buffer;
    try {
        gt.recv(&p, 13);
        BOOST_CHECK(false);
    } catch (Error & e) {
        BOOST_CHECK_EQUAL(p-buffer, 12);
        BOOST_CHECK_EQUAL(0, strncmp(buffer, " we provide!", 12));
        BOOST_CHECK_EQUAL(e.id, static_cast<int>(ERR_TRANSPORT_NO_MORE_DATA));
    }
}

BOOST_AUTO_TEST_CASE(TestCheckTransport)
{
    CheckTransport gt("input", 5);
    gt.disable_remaining_error();
    BOOST_CHECK_EQUAL(gt.get_status(), true);
    try{
        gt.send("in", 2);
    } catch (const Error &){
        BOOST_CHECK(false);
    }
    BOOST_CHECK_EQUAL(gt.get_status(), true);
    try{
        gt.send("pot", 3);
    } catch (const Error & e){
        BOOST_CHECK_EQUAL(ERR_TRANSPORT_DIFFERS, e.id);
    }
    BOOST_CHECK(!gt.get_status());
}

BOOST_AUTO_TEST_CASE(TestCheckTransportInputOverflow)
{
    CheckTransport gt("0123456789ABCDEF", 16);
    BOOST_CHECK_EQUAL(gt.get_status(), true);
    try {
        gt.send("0123456789ABCDEFGHI", 19);
    } catch (const Error & e)
    {
        BOOST_CHECK_EQUAL(ERR_TRANSPORT_DIFFERS, e.id);
    }
    BOOST_CHECK(!gt.get_status());
}

BOOST_AUTO_TEST_CASE(TestTestTransport)
{
    // TestTransport is bidirectional
    // We provide both an output and an input source
    // when using a test Transport we read what we provide in input source
    // and we check that what we write to output is identical to output source
    // if send fails, the difference between expected and actual data is showed
    // and status is set to false (and will stay so) to allow tests to fail.
    // inside Transport, the difference is shown in trace logs.
    TestTransport gt("Test1", "OUTPUT", 6, "input", 5);
    gt.disable_remaining_error();
    BOOST_CHECK_EQUAL(gt.get_status(), true);
    char buf[128] = {};
    char * p = buf;
    uint32_t sz = 3;
    gt.recv(&p, sz);
    BOOST_CHECK(0 == memcmp(p - sz, "OUT", sz));
    gt.send("in", 2);
    BOOST_CHECK_EQUAL(gt.get_status(), true);
    sz = 3;
    gt.recv(&p, sz);
    BOOST_CHECK(0 == memcmp(p - sz, "PUT", sz));
    try {
        gt.send("pot", 3);
    } catch (const Error & e){
        BOOST_CHECK_EQUAL(ERR_TRANSPORT_DIFFERS, e.id);
    }
    BOOST_CHECK(!gt.get_status());
}

BOOST_AUTO_TEST_CASE(TestMemoryTransport)
{
    MemoryTransport mt;

    char     s_data[]    = "0123456789ABCDEF";
    uint32_t s_data_size = strlen(s_data);

    mt.send(reinterpret_cast<char *>(&s_data_size), sizeof(s_data_size));
    mt.send(s_data, s_data_size);

    char     r_data[32]  = { 0 };
    uint32_t r_data_size = 0;

    char * r_buffer = reinterpret_cast<char *>(&r_data_size);
    mt.recv(&r_buffer, sizeof(uint32_t));
    BOOST_CHECK_EQUAL(r_data_size, s_data_size);
    //LOG(LOG_INFO, "r_data_size=%u", r_data_size);

    r_buffer = r_data;
    mt.recv(&r_buffer, r_data_size);
    BOOST_CHECK_EQUAL(memcmp(r_data, s_data, r_data_size), 0);
    //LOG(LOG_INFO, "r_data=\"%s\"", r_data);
}
