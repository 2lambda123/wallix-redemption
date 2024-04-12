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
 *   Copyright (C) Wallix 2010-2013
 *   Author(s): Christophe Grosjean, Dominique Lafages, Jonathan Poelen,
 *              Meng Tan
 */

#include "mod/internal/widget/number_edit.hpp"
#include "keyboard/keymap.hpp"

WidgetNumberEdit::WidgetNumberEdit(
    gdi::GraphicApi & drawable, CopyPaste & copy_paste,
    chars_view text, WidgetEventNotifier onsubmit,
    Color fgcolor, Color bgcolor, Color focus_color,
    Font const & font, int xtext, int ytext)
: WidgetEdit(drawable, copy_paste, text, onsubmit,
             fgcolor, bgcolor, focus_color, font, xtext, ytext)
{}

void WidgetNumberEdit::set_text(chars_view text)
{
    this->WidgetEdit::set_text(text);
}

void WidgetNumberEdit::insert_text(chars_view text)
{
    for (char c : text) {
        if (c < '0' || '9' < c) {
            return ;
        }
    }
    WidgetEdit::insert_text(text);
}

void WidgetNumberEdit::rdp_input_scancode(KbdFlags flags, Scancode scancode, uint32_t event_time, Keymap const& keymap)
{
    if (keymap.last_kevent() == Keymap::KEvent::KeyDown) {
        auto c = keymap.last_decoded_keys().uchars[0];
        if (c < '0' || '9' < c) {
            return ;
        }
    }
    WidgetEdit::rdp_input_scancode(flags, scancode, event_time, keymap);
}
