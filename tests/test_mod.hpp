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
   Copyright (C) Wallix 2011
   Author(s): Christophe Grosjean, Martin Potier

   Unit test of Modules API

*/

#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_MODULE TestMod
#include <boost/test/auto_unit_test.hpp>

BOOST_AUTO_TEST_CASE(TestNullModule)
{

// Null module receive every event and does nothing.
// It allows session code to always have a receiving module active,
// thus avoidind to test that some back_end is available.




}

#endif
