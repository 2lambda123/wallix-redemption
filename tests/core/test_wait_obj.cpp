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
   Author(s): Christophe Grosjean, Meng Tan

*/

#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestXXX
#include <boost/test/auto_unit_test.hpp>

#define LOGNULL

#include "wait_obj.hpp"
#include "socket_transport_utility.hpp"

BOOST_AUTO_TEST_CASE(Testwait_obj)
{
    wait_obj nonsocketobj;
    unsigned max = 0;

    // nonsocketobj never interact with system fd_set and max;
    fd_set rfds;
    fd_set wfds;
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    struct timeval timeout = { 2L, 0L };

    bool res;


    // initialy, wait obj is not set
    res = is_set(nonsocketobj, nullptr, rfds);
    BOOST_CHECK_EQUAL(res, false);

    nonsocketobj.reset();
    res = is_set(nonsocketobj, nullptr, rfds);
    BOOST_CHECK_EQUAL(res, false);

    // add to fd set does not change anything on
    // timeout if the wait obj is not set
    add_to_fd_set(nonsocketobj, nullptr, rfds, max, timeout);
    BOOST_CHECK_EQUAL(timeout.tv_sec, 2L);
    BOOST_CHECK_EQUAL(timeout.tv_usec, 0L);


    // set wait obj (no timer)
    nonsocketobj.set();
    res = is_set(nonsocketobj, nullptr, rfds);
    BOOST_CHECK_EQUAL(res, true);

    // adding it to fd set change the time out
    add_to_fd_set(nonsocketobj, nullptr, rfds, max, timeout);
    BOOST_CHECK_EQUAL(timeout.tv_sec, 0L);
    BOOST_CHECK_EQUAL(timeout.tv_usec, 0L);
    timeout.tv_sec = 2L;

    // reset
    nonsocketobj.reset();
    res = is_set(nonsocketobj, nullptr, rfds);
    BOOST_CHECK_EQUAL(res, false);

    // set a waitobj with a timer will set it
    // after the timeout is triggered
    nonsocketobj.set(500000);
    res = is_set(nonsocketobj, nullptr, rfds);
    BOOST_CHECK_EQUAL(res, false);
    BOOST_CHECK_EQUAL(timeout.tv_sec, 2L);
    BOOST_CHECK_EQUAL(timeout.tv_usec, 0);

    // adding it to fd set while it has a timer to be set
    // will change de timeout to the remaining time to be set
    // if the timeout is longer than this remaining time.
    add_to_fd_set(nonsocketobj, nullptr, rfds,max,timeout);
    BOOST_CHECK_EQUAL(timeout.tv_sec, 0L);
    BOOST_CHECK_EQUAL((timeout.tv_usec <= 500000L) &&
                      (timeout.tv_usec > 200000L), true);

    timeout.tv_sec = 2L;
    // adding two non socket waitobj to fd set
    // setting them with differents timers.
    wait_obj nonsocketobj2;
    nonsocketobj.set(1500000);
    nonsocketobj2.set(300000);

    add_to_fd_set(nonsocketobj, nullptr, rfds,max,timeout);
    BOOST_CHECK_EQUAL(timeout.tv_sec, 1L);
    BOOST_CHECK_EQUAL((timeout.tv_usec <= 500000L) &&
                      (timeout.tv_usec > 0L), true);
    add_to_fd_set(nonsocketobj2, nullptr, rfds,max,timeout);
    BOOST_CHECK_EQUAL(timeout.tv_sec, 0L);
    BOOST_CHECK_EQUAL((timeout.tv_usec <= 300000L) &&
                      (timeout.tv_usec > 0L), true);


    // set a wait obj twice but with different timers:
    // only the last timer will be considered
    timeout.tv_sec = 2L;
    nonsocketobj.set(300000);
    res = is_set(nonsocketobj, nullptr, rfds);
    BOOST_CHECK_EQUAL(res, false);
    nonsocketobj.set(1400000);
    res = is_set(nonsocketobj, nullptr, rfds);
    BOOST_CHECK_EQUAL(res, false);

    add_to_fd_set(nonsocketobj, nullptr, rfds,max,timeout);
    BOOST_CHECK_EQUAL(timeout.tv_sec, 1L);
    BOOST_CHECK_EQUAL((timeout.tv_usec <= 500000L) &&
                      (timeout.tv_usec > 200000L), true);


    // update a wait obj twice (or many times) will consider the closest timer.
    // test1
    timeout.tv_sec = 2L;
    timeout.tv_usec = 0L;

    nonsocketobj.reset();
    nonsocketobj.update(300000);
    res = is_set(nonsocketobj, nullptr, rfds);
    BOOST_CHECK_EQUAL(res, false);
    nonsocketobj.update(1400000);
    res = is_set(nonsocketobj, nullptr, rfds);
    BOOST_CHECK_EQUAL(res, false);

    add_to_fd_set(nonsocketobj, nullptr, rfds,max,timeout);
    BOOST_CHECK_EQUAL(timeout.tv_sec, 0L);
    BOOST_CHECK_EQUAL((timeout.tv_usec <= 300000L) &&
                      (timeout.tv_usec > 0L), true);

    // test2
    timeout.tv_sec = 2L;
    timeout.tv_usec = 0L;

    nonsocketobj.reset();
    nonsocketobj.update(1400000);
    res = is_set(nonsocketobj, nullptr, rfds);
    BOOST_CHECK_EQUAL(res, false);
    nonsocketobj.update(800000);
    res = is_set(nonsocketobj, nullptr, rfds);
    BOOST_CHECK_EQUAL(res, false);


    add_to_fd_set(nonsocketobj, nullptr, rfds,max,timeout);
    BOOST_CHECK_EQUAL(timeout.tv_sec, 0L);
    BOOST_CHECK_EQUAL((timeout.tv_usec <= 800000L) &&
                      (timeout.tv_usec > 500000L), true);

    // test 3
    timeout.tv_sec = 3L;
    timeout.tv_usec = 0L;

    nonsocketobj.reset();
    nonsocketobj.update(1400000);
    res = is_set(nonsocketobj, nullptr, rfds);
    BOOST_CHECK_EQUAL(res, false);
    nonsocketobj.update(2800000);
    res = is_set(nonsocketobj, nullptr, rfds);
    BOOST_CHECK_EQUAL(res, false);


    add_to_fd_set(nonsocketobj, nullptr, rfds,max,timeout);
    BOOST_CHECK_EQUAL(timeout.tv_sec, 1L);
    BOOST_CHECK_EQUAL((timeout.tv_usec <= 400000L) &&
                      (timeout.tv_usec > 100000L), true);
}
