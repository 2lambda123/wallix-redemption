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
 *   Copyright (C) Wallix 2010-2012
 *   Author(s): Christophe Grosjean, Dominique Lafages, Jonathan Poelen,
 *              Meng Tan
 */

#include "test_only/test_framework/redemption_unit_tests.hpp"
#include "test_only/test_framework/check_img.hpp"

#include "mod/internal/copy_paste.hpp"
#include "mod/internal/widget/number_edit.hpp"
#include "keyboard/keymap.hpp"
#include "keyboard/keylayouts.hpp"
#include "test_only/gdi/test_graphic.hpp"
#include "test_only/core/font.hpp"
#include "test_only/mod/internal/widget/notify_trace.hpp"


#define IMG_TEST_PATH FIXTURES_PATH "/img_ref/mod/internal/widget/number_edit/"


RED_AUTO_TEST_CASE(WidgetNumberEditEventPushChar)
{
    TestGraphic drawable(800, 600);
    CopyPaste copy_paste(false);

    int16_t x = 0;
    int16_t y = 0;
    uint16_t cx = 100;

    NotifyTrace onsubmit;

    WidgetNumberEdit wnumber_edit(
        drawable, copy_paste, "123456"_av, onsubmit,
        GREEN, RED, RED, global_font_deja_vu_14());
    Dimension dim = wnumber_edit.get_optimal_dim();
    wnumber_edit.set_wh(cx, dim.h);
    wnumber_edit.set_xy(x, y);
    wnumber_edit.focus(Widget::focus_reason_tabkey);

    wnumber_edit.rdp_input_invalidate(wnumber_edit.get_rect());
    RED_CHECK_IMG(drawable, IMG_TEST_PATH "number_edit_1.png");

    Keymap keymap(*find_layout_by_id(KeyLayout::KbdId(0x040C)));
    using KFlags = Keymap::KbdFlags;
    using Scancode = Keymap::Scancode;

    keymap.event(KFlags(), Scancode(0x10)); // 'a'
    wnumber_edit.rdp_input_scancode(KFlags(), Scancode(0x10), 0, keymap);
    RED_CHECK_IMG(drawable, IMG_TEST_PATH "number_edit_1.png");
    RED_CHECK(onsubmit.get_and_reset() == 0);

    keymap.event(KFlags(), Scancode(0x2a)); // shift
    wnumber_edit.rdp_input_scancode(KFlags(), Scancode(0x2a), 0, keymap);
    keymap.event(KFlags(), Scancode(0x03)); // '2'
    wnumber_edit.rdp_input_scancode(KFlags(), Scancode(0x03), 0, keymap);
    RED_CHECK_IMG(drawable, IMG_TEST_PATH "number_edit_3.png");
    RED_CHECK(onsubmit.get_and_reset() == 0);
}
