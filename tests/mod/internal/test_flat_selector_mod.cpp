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
#define BOOST_TEST_MODULE TestFlatSelectorMod
#include <boost/test/auto_unit_test.hpp>

#define LOGNULL
#include "log.hpp"

#include "internal/flat_selector_mod.hpp"
#include "../../front/fake_front.hpp"

BOOST_AUTO_TEST_CASE(TestSelectorMod)
{
    ClientInfo info;
    info.keylayout = 0x040C;
    info.console_session = 0;
    info.brush_cache_code = 0;
    info.bpp = 24;
    info.width = 800;
    info.height = 600;

    FakeFront front(info, 0);

    Inifile             ini;

    Keymap2 keymap;

    TODO("provide data for selector and write an actual scenarized test (use case)")

    FlatSelectorMod d(ini, front, 800, 600);

    ini.globals.target_user.set_from_acl("tartempion");
    ini.globals.target_device.set_from_acl("K2000");
    ini.context.target_protocol.set_from_acl("TCP/IP");
    ini.context.end_time.set_from_acl("never");

    ini.context.selector_lines_per_page.set_from_acl("1");

    ini.context.selector_current_page.set_from_acl("1");
    ini.context.selector_number_of_pages.set_from_acl("2");
    d.refresh_context(ini);

    d.refresh_device();

    keymap.init_layout(info.keylayout);
    keymap.push_kevent(Keymap2::KEVENT_LEFT_ARROW);
    d.rdp_input_scancode(0, 0, 0, 0, &keymap);
    keymap.push_kevent(Keymap2::KEVENT_LEFT_ARROW);
    d.rdp_input_scancode(0, 0, 0, 0, &keymap);

    keymap.push_kevent(Keymap2::KEVENT_RIGHT_ARROW);
    d.rdp_input_scancode(0, 0, 0, 0, &keymap);
    keymap.push_kevent(Keymap2::KEVENT_RIGHT_ARROW);
    d.rdp_input_scancode(0, 0, 0, 0, &keymap);

    keymap.push_kevent(Keymap2::KEVENT_ENTER);
    d.rdp_input_scancode(0, 0, 0, 0, &keymap);

}

