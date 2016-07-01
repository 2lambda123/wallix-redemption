/*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*   Product name: redemption, a FLOSS RDP proxy
*   Copyright (C) Wallix 2010-2014
*   Author(s): Christophe Grosjean, Raphael Zhou, Jonathan Poelen, Meng Tan
*/

#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK

#define BOOST_TEST_MODULE TestArrayView
#include "system/redemption_unit_tests.hpp"

#include "utils/sugar/array_view.hpp"

#include <string>

namespace {

int test_ambiguous(array_view_const_char) { return 1; }
int test_ambiguous(array_view_const_s8) { return 2; }
int test_ambiguous(array_view_const_u8) { return 3; }

}

BOOST_AUTO_TEST_CASE(TestArrayView)
{
    char a8[3] = {'x', 'y', 'z'};
    int8_t as8[3] = {-1, 0, 3};
    uint8_t au8[3] = {0, 1, 2};

    BOOST_CHECK_EQUAL(test_ambiguous(a8), 1);
    BOOST_CHECK_EQUAL(test_ambiguous(as8), 2);
    BOOST_CHECK_EQUAL(test_ambiguous(au8), 3);

    BOOST_CHECK_EQUAL(make_array_view(a8).size(), 3);

    std::string s;
    BOOST_CHECK_EQUAL(test_ambiguous(s), 1);

    s = "abc";
    array_view<const char> avc(s);
    // same size (as std::string)
    BOOST_CHECK_EQUAL(avc.size(), s.size());
    // same data (same memory address)
    BOOST_CHECK_EQUAL(avc.data(), s.data());

    auto av = make_array_view(s);
    // same type as s
    BOOST_CHECK_EQUAL(test_ambiguous(av), 1);
    // same size (as std::string)
    BOOST_CHECK_EQUAL(av.size(), s.size());
    // same data (same memory address)
    BOOST_CHECK_EQUAL(av.data(), s.data());
    BOOST_CHECK_EQUAL(av[0], s[0]);
    BOOST_CHECK_EQUAL(av[1], s[1]);
    BOOST_CHECK_EQUAL(av[2], s[2]);
    // array view provides begin() and end()
    BOOST_CHECK_EQUAL(av.end() - av.begin(), 3);
    // begin is an iterator to first char
    BOOST_CHECK_EQUAL(*av.begin(), 'a');
    auto it = av.begin();
    it++;
    BOOST_CHECK_EQUAL(*it, 'b');

    auto const av_p = make_array_view(&s[0], &s[3]);
    BOOST_CHECK_EQUAL(static_cast<void const *>(av_p.data()), static_cast<void const *>(av.data()));
    BOOST_CHECK_EQUAL(av_p.size(), av.size());
    BOOST_CHECK_EQUAL(av_p[0], av[0]);
    BOOST_CHECK_EQUAL(av_p[1], s[1]);
    BOOST_CHECK_EQUAL(av_p[2], s[2]);
    // array view provides begin() and end()
    BOOST_CHECK_EQUAL(av_p.end() - av_p.begin(), 3);

    // begin is an iterator to first char
    BOOST_CHECK_EQUAL(*av_p.begin(), 'a');
    auto it2 = av_p.begin();
    it2++;
    BOOST_CHECK_EQUAL(*it2, 'b');

    BOOST_CHECK_EQUAL(make_array_view("abc").size(), 4);
    BOOST_CHECK_EQUAL(cstr_array_view("abc").size(), 3);
    BOOST_CHECK_EQUAL(make_array_view(av.data(), 1).size(), 1);

    BOOST_CHECK(array_view_char{nullptr}.empty());

    {
    char ca8[3] = {'x', 'y', 'z'};
    const char * left = &ca8[1];
    char * right = &ca8[2];
    auto const avi = make_array_view(left, right);

    BOOST_CHECK_EQUAL(avi.size(), 1);
    BOOST_CHECK_EQUAL(reinterpret_cast<const void*>(avi.data()),
                      reinterpret_cast<const void*>(&ca8[1]));
    }

    {
    char ca8[3] = {'x', 'y', 'z'};
    char * left = &ca8[1];
    const char * right = &ca8[2];
    auto const avi = make_array_view(left, right);

    BOOST_CHECK_EQUAL(avi.size(), 1);
    BOOST_CHECK_EQUAL(reinterpret_cast<const void*>(avi.data()),
                      reinterpret_cast<const void*>(&ca8[1]));
    }

    {
    char ca8[] = {'x', 'y', 'z', 't'};
    const char * left = &ca8[1];
    auto const avi = make_const_array_view(left, 2);

    BOOST_CHECK_EQUAL(avi.size(), 2);
    BOOST_CHECK_EQUAL(reinterpret_cast<const void*>(avi.data()),
                      reinterpret_cast<const void*>(&ca8[1]));
    }

    {
    char ca8[] = {'x', 'y', 'z', 't'};
    const char * left = &ca8[1];
    const char * right = &ca8[1];
    auto const avi = make_const_array_view(left, right);

    BOOST_CHECK_EQUAL(avi.size(), 0);
    BOOST_CHECK_EQUAL(reinterpret_cast<const void*>(avi.data()),
                      reinterpret_cast<const void*>(&ca8[1]));
    }

    {
    const char ca8[] = {'x', 'y', 'z', 't'};
    auto const avi = make_const_array_view(ca8);

    BOOST_CHECK_EQUAL(avi.size(), 4);
    BOOST_CHECK_EQUAL(reinterpret_cast<const void*>(avi.data()),
                      reinterpret_cast<const void*>(&ca8[0]));
    }

    {
    auto const avi = cstr_array_view("0123456789");
    BOOST_CHECK_EQUAL(avi.size(), 10);
//    BOOST_CHECK_EQUAL(reinterpret_cast<const void*>(avi.data()),
//                      reinterpret_cast<const void*>(&ca8[0]));
    }
}

template<class T>
auto check_call(T && a, int) -> decltype(cstr_array_view(a), true)
{
    return true;
}


template<class T>
bool check_call(T && a, char)
{
    return false;
}

BOOST_AUTO_TEST_CASE(TestCStrOnlyWorksForLitterals)
{
    char cstr[5] = {'0', '1', '2', '\0', '5'};
    BOOST_CHECK_EQUAL(check_call(cstr, 1), false);
    BOOST_CHECK_EQUAL(check_call("abc", 1), true);
    char const * p = nullptr;
    BOOST_CHECK_EQUAL(check_call(p, 1), false);
}
