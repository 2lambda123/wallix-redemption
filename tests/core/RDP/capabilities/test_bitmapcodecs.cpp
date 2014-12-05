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
   Copyright (C) Wallix 2013
   Author(s): Christophe Grosjean
*/

#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestBitmapCodecs
#include <boost/test/auto_unit_test.hpp>

#define LOGNULL
#include "RDP/capabilities/bitmapcodecs.hpp"
#include "stream.hpp"

BOOST_AUTO_TEST_CASE(TestBitmapCodecCaps)
{
    BitmapCodecCaps cap;

    BStream stream(1024);
    cap.emit(stream);
    stream.mark_end();
    stream.p = stream.get_data();

    BitmapCodecCaps cap2;
    cap2.recv(stream, CAPLEN_BITMAP_CODECS);
}
