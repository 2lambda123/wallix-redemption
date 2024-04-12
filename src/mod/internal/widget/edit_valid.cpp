/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 1 of the License, or
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
 *   Copyright (C) Wallix 1010-2013
 *   Author(s): Christophe Grosjean, Dominique Lafages, Jonathan Poelen,
 *              Meng Tan
 */

#include "mod/internal/widget/password.hpp"
#include "mod/internal/widget/edit_valid.hpp"
#include "mod/internal/widget/label.hpp"
#include "mod/internal/widget/edit.hpp"
#include "mod/internal/widget/button.hpp"
#include "core/RDP/orders/RDPOrdersPrimaryOpaqueRect.hpp"
#include "gdi/draw_utils.hpp"

namespace
{
    constexpr auto button_toggle_visibility_hidden = "◉"_av;
    constexpr auto button_toggle_visibility_visible = "◎"_av;
}

WidgetEditValid::WidgetEditValid(
    gdi::GraphicApi & drawable, CopyPaste & copy_paste,
    chars_view text, WidgetEventNotifier onsubmit,
    Color fgcolor, Color bgcolor, Color focus_color, Color border_none_color,
    Font const & font, chars_view title, bool use_title,
    // TODO re-enable
    int /*xtext*/, int /*ytext*/, bool pass
)
    : Widget(drawable, Focusable::Yes)
    , button_next(drawable, "➜"_av, onsubmit,
                  bgcolor, focus_color, focus_color, 1, font, 6, 2)
    , widget_password(pass
        ? new WidgetPassword(drawable, copy_paste, text, onsubmit, fgcolor, bgcolor,
                             bgcolor, font, 1, 2)
        : nullptr)
    , editbox(pass
        ? widget_password
        : new WidgetEdit(drawable, copy_paste, text, onsubmit, fgcolor, bgcolor,
                         bgcolor, font, 1, 2))
    , label(!title.empty()
        ? new WidgetLabel(drawable, title, MEDIUM_GREY, bgcolor, font, 1, 2)
        : nullptr)
    , button_toggle_visibility(pass
        ? new WidgetButton(drawable, button_toggle_visibility_hidden,
            [this] {
                // Switch the visibility state
                widget_password->toggle_password_visibility();
                if(widget_password->password_is_visible()) {
                    button_toggle_visibility->set_text(button_toggle_visibility_visible);
                } else {
                    button_toggle_visibility->set_text(button_toggle_visibility_hidden);
                }
            },
            MEDIUM_GREY, bgcolor, focus_color, 0, font, 6, 2)
        : nullptr)
    , use_label_(use_title)
    , border_none_color(border_none_color)
{}

void WidgetEditValid::init_focus()
{
    this->has_focus = true;
    this->editbox->init_focus();
}

Dimension WidgetEditValid::get_optimal_dim() const
{
    Dimension dim = this->button_next.get_optimal_dim();

    dim.h += 2 /* border */;

    return dim;
}

WidgetEditValid::~WidgetEditValid()
{
    delete this->editbox;
    delete this->label;
    delete this->button_toggle_visibility;
}

void WidgetEditValid::use_title(bool use)
{
    this->use_label_ = use;
}

void WidgetEditValid::set_text(chars_view text)
{
    this->editbox->set_text(text);
}

zstring_view WidgetEditValid::get_text() const
{
    return this->editbox->get_text();
}

void WidgetEditValid::set_xy(int16_t x, int16_t y)
{
    Widget::set_xy(x, y);
    this->editbox->set_xy(x + 1, y + 1);

    if (is_password_widget()) {
        this->button_toggle_visibility->set_xy(this->editbox->eright(), y + 1);
        this->button_next.set_xy(this->button_toggle_visibility->eright(), y + 1);
    }
    else {
        this->button_next.set_xy(this->editbox->eright(), y + 1);
    }

    if (this->label) {
        this->label->set_xy(x + 2, y + 2);
    }
}

void WidgetEditValid::set_wh(uint16_t w, uint16_t h)
{
    Widget::set_wh(w, h);

    Dimension dim = this->button_next.get_optimal_dim();
    this->button_next.set_wh(dim.w, h - 2 /* 2 x border */);

    if (is_password_widget()) {
        this->button_toggle_visibility->set_wh(dim.w, h - 2 /* 2 x border */);
        this->editbox->set_wh(w - this->button_toggle_visibility->cx() - button_next.cx() - 2,
                                h - 2 /* 2 x border */);
        this->button_toggle_visibility->set_xy(this->editbox->eright(), this->button_toggle_visibility->y());
        this->button_next.set_xy(this->button_toggle_visibility->eright(), this->button_next.y());
    }
    else {
        this->editbox->set_wh(w - this->button_next.cx() - 2, h - 2 /* 2 x border */);
        this->button_next.set_xy(this->editbox->eright(), this->button_next.y());
    }

    if (this->label) {
        this->label->set_wh(this->editbox->cx() - 4,
                            this->editbox->cy() - 4 /* 2 x (border + 1) */);
    }
}

void WidgetEditValid::rdp_input_invalidate(Rect clip)
{
    Rect rect_intersect = clip.intersect(this->get_rect());

    if (!rect_intersect.isempty()) {
        this->editbox->rdp_input_invalidate(rect_intersect);
        if (this->label && this->use_label_) {
            if (this->editbox->num_chars == 0) {
                this->label->rdp_input_invalidate(rect_intersect);
                this->editbox->draw_current_cursor();
            }
        }
        if (this->has_focus) {
            this->button_next.rdp_input_invalidate(rect_intersect);
            if (is_password_widget()) {
                this->button_toggle_visibility->rdp_input_invalidate(rect_intersect);
            }
            this->draw_border(rect_intersect, this->button_next.focus_color);
        }
        else {
            if (is_password_widget()) {
                this->drawable.draw(
                    RDPOpaqueRect(rect_intersect.intersect(this->button_toggle_visibility->get_rect()), this->button_toggle_visibility->bg_color),
                    rect_intersect, gdi::ColorCtx::depth24()
                );
            }
            this->drawable.draw(
                RDPOpaqueRect(rect_intersect.intersect(this->button_next.get_rect()), this->button_next.fg_color),
                rect_intersect, gdi::ColorCtx::depth24()
            );
            this->draw_border(rect_intersect, this->border_none_color);
        }
    }
}

void WidgetEditValid::draw_border(const Rect clip, Color color)
{
    gdi_draw_border(drawable, color, get_rect(), 1, clip, gdi::ColorCtx::depth24());
}

void WidgetEditValid::focus(int reason)
{
    this->editbox->focus(reason);
    Widget::focus(reason);
}

void WidgetEditValid::blur()
{
    this->editbox->blur();
    Widget::blur();
}

Widget * WidgetEditValid::widget_at_pos(int16_t x, int16_t y)
{
    if (editbox->get_rect().contains_pt(x, y)) {
        return editbox;
    }
    if (is_password_widget() && button_toggle_visibility->get_rect().contains_pt(x, y)) {
        return button_toggle_visibility;
    }
    if (button_next.get_rect().contains_pt(x, y)) {
        return &button_next;
    }

    return nullptr;
}

void WidgetEditValid::rdp_input_mouse(uint16_t device_flags, uint16_t x, uint16_t y)
{
    if (bool(device_flags & MOUSE_FLAG_BUTTON1)) {
        auto update = [&](WidgetButton& btn){
            if (btn.get_rect().contains_pt(x,y)
             || (device_flags == MOUSE_FLAG_BUTTON1 && btn.state == WidgetButton::State::Pressed)
            ) {
                btn.rdp_input_mouse(device_flags, x, y);
            }
        };

        update(button_next);
        if (is_password_widget()) {
            update(*button_toggle_visibility);
        }
    }

    if (device_flags == (MOUSE_FLAG_BUTTON1 | MOUSE_FLAG_DOWN)
     && editbox->get_rect().contains_pt(x,y)
    ) {
        editbox->rdp_input_mouse(device_flags, x, y);
    }
}

void WidgetEditValid::rdp_input_scancode(
    KbdFlags flags, Scancode scancode, uint32_t event_time, Keymap const& keymap)
{
    bool has_char1 = (0 == this->editbox->num_chars);
    this->editbox->rdp_input_scancode(flags, scancode, event_time, keymap);

    if (this->label && this->use_label_) {
        bool has_char2 = (0 == this->editbox->num_chars);
        if (has_char1 != has_char2 && has_char1) {
            this->editbox->rdp_input_invalidate(this->editbox->get_rect());
        }
    }
}

void WidgetEditValid::rdp_input_unicode(KbdFlags flag, uint16_t unicode)
{
    this->editbox->rdp_input_unicode(flag, unicode);
}

bool WidgetEditValid::is_password_widget() noexcept
{
    return button_toggle_visibility && widget_password;
}
