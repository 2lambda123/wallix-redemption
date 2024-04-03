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

#include "mod/internal/widget/form.hpp"
#include "utils/sugar/chars_to_int.hpp"
#include "utils/trkeys.hpp"
#include "utils/theme.hpp"

using namespace std::chrono_literals;

WidgetForm::WidgetForm(
    gdi::GraphicApi & drawable, CopyPaste & copy_paste,
    int16_t left, int16_t top, int16_t width, int16_t height,
    Events events,
    Font const & font, Theme const & theme, Language lang,
    unsigned flags, std::chrono::minutes duration_max
)
    : WidgetForm(drawable, copy_paste, events,
                 font, theme, lang, flags, duration_max)
{
    this->move_size_widget(left, top, width, height);
}

WidgetForm::WidgetForm(
    gdi::GraphicApi & drawable, CopyPaste & copy_paste,
    Events events,
    Font const & font, Theme const & theme, Language lang,
    unsigned flags, std::chrono::minutes duration_max
)
    : WidgetComposite(drawable, Focusable::Yes)
    , events(events)
    , warning_msg(drawable, "",
                  theme.global.error_color, theme.global.bgcolor, font)
    , duration_label(drawable, TR(trkeys::duration, lang).to_sv(),
                     theme.global.fgcolor, theme.global.bgcolor, font)
    , duration_edit(drawable, copy_paste, nullptr,
                    {[this]{ this->check_confirmation(); }},
                    theme.edit.fgcolor, theme.edit.bgcolor,
                    theme.edit.focus_color, font, -1, 1, 1)
    , duration_format(drawable, TR(trkeys::note_duration_format, lang).to_sv(),
                      theme.global.fgcolor, theme.global.bgcolor, font)
    , ticket_label(drawable, TR(trkeys::ticket, lang).to_sv(),
                   theme.global.fgcolor, theme.global.bgcolor, font)
    , ticket_edit(drawable, copy_paste, nullptr,
                  {[this]{ this->check_confirmation(); }},
                  theme.edit.fgcolor, theme.edit.bgcolor,
                  theme.edit.focus_color, font, -1, 1, 1)
    , comment_label(drawable, TR(trkeys::comment, lang).to_sv(),
                    theme.global.fgcolor, theme.global.bgcolor, font)
    , comment_edit(drawable, copy_paste, nullptr,
                   {[this]{ this->check_confirmation(); }},
                   theme.edit.fgcolor, theme.edit.bgcolor,
                   theme.edit.focus_color, font, -1, 1, 1)
    , notes(drawable, TR(trkeys::note_required, lang).to_sv(),
            theme.global.fgcolor, theme.global.bgcolor, font)
    , confirm(drawable, TR(trkeys::confirm, lang),
              [this]{ return this->check_confirmation(); },
              theme.global.fgcolor, theme.global.bgcolor, theme.global.focus_color,
              2, font, 6, 2)
    , tr(lang)
    , flags(flags)
    , duration_max(duration_max == 0min ? 60000min : duration_max)
    , warning_buffer()
{
    this->set_bg_color(theme.global.bgcolor);

    this->add_widget(this->warning_msg);

    if (this->flags & DURATION_DISPLAY) {
        this->add_widget(this->duration_label);
        this->add_widget(this->duration_edit);
        this->add_widget(this->duration_format);
    }
    if (this->flags & TICKET_DISPLAY) {
        this->add_widget(this->ticket_label);
        this->add_widget(this->ticket_edit);
    }
    if (this->flags & COMMENT_DISPLAY) {
        this->add_widget(this->comment_label);
        this->add_widget(this->comment_edit);
    }
    if (this->flags & DURATION_MANDATORY) {
        this->duration_label.set_text(TR(trkeys::duration_r, lang).to_sv());
    }
    if (this->flags & TICKET_MANDATORY) {
        this->ticket_label.set_text(TR(trkeys::ticket_r, lang).to_sv());
    }
    if (this->flags & COMMENT_MANDATORY) {
        this->comment_label.set_text(TR(trkeys::comment_r, lang).to_sv());
    }

    if (this->flags & (COMMENT_MANDATORY | TICKET_MANDATORY | DURATION_MANDATORY)) {
        this->add_widget(this->notes);
    }

    this->add_widget(this->confirm);
}

void WidgetForm::move_size_widget(int16_t left, int16_t top, uint16_t width, uint16_t height)
{
    this->set_xy(left, top);
    this->set_wh(width, height);

    Dimension dim;

    uint16_t labelmaxwidth = 0;

    if (this->flags & DURATION_DISPLAY) {
        dim = this->duration_label.get_optimal_dim();
        this->duration_label.set_wh(dim);

        labelmaxwidth = std::max(labelmaxwidth, this->duration_label.cx());
    }

    if (this->flags & TICKET_DISPLAY) {
        dim = this->ticket_label.get_optimal_dim();
        this->ticket_label.set_wh(dim);

        labelmaxwidth = std::max(labelmaxwidth, this->ticket_label.cx());
    }

    if (this->flags & COMMENT_DISPLAY) {
        dim = this->comment_label.get_optimal_dim();
        this->comment_label.set_wh(dim);

        labelmaxwidth = std::max(labelmaxwidth, this->comment_label.cx());
    }

    dim = this->warning_msg.get_optimal_dim();
    this->warning_msg.set_wh(width - labelmaxwidth - 20, dim.h);
    this->warning_msg.set_xy(left + labelmaxwidth + 20, top);

    int y = 20;

    if (this->flags & DURATION_DISPLAY) {
        this->duration_label.set_xy(left, top + y);

        dim = this->duration_format.get_optimal_dim();
        this->duration_format.set_wh(dim);

        dim = this->duration_edit.get_optimal_dim();
        this->duration_edit.set_wh((width - labelmaxwidth - 20) - this->duration_format.cx() - 20,
                                    dim.h);
        this->duration_edit.set_xy(left + labelmaxwidth + 20, top + y);

        this->duration_format.set_xy(this->duration_edit.eright() + 10, top + y + 2);

        y += 30;
    }

    if (this->flags & TICKET_DISPLAY) {
        this->ticket_label.set_xy(left, top + y);

        dim = this->ticket_edit.get_optimal_dim();
        this->ticket_edit.set_wh(width - labelmaxwidth - 20, dim.h);
        this->ticket_edit.set_xy(left + labelmaxwidth + 20, top + y);

        y += 30;
    }

    if (this->flags & COMMENT_DISPLAY) {
        this->comment_label.set_xy(left, top + y);

        dim = this->comment_edit.get_optimal_dim();
        this->comment_edit.set_wh(width - labelmaxwidth - 20, dim.h);
        this->comment_edit.set_xy(left + labelmaxwidth + 20, top + y);

        y += 30;
    }

    if (this->flags & (COMMENT_MANDATORY | TICKET_MANDATORY | DURATION_MANDATORY)) {
        dim = this->notes.get_optimal_dim();
        this->notes.set_wh(width - labelmaxwidth - 20, dim.h);
        this->notes.set_xy(left + labelmaxwidth + 20, top + y);
    }

    dim = this->confirm.get_optimal_dim();
    this->confirm.set_wh(dim);
    this->confirm.set_xy(left + width - this->confirm.cx(), top + y + 10);
}

namespace
{
    template<class T>
    T const& to_ctype(T const& x)
    {
        return x;
    }

    char const* to_ctype(zstring_view const& str)
    {
        return str.c_str();
    }
} // anonymous namespace

template<class T, class... Ts>
void WidgetForm::set_warning_buffer(TrKeyFmt<T> k, Ts const&... xs)
{
    tr.fmt(this->warning_buffer, sizeof(this->warning_buffer), k, to_ctype(xs)...);
    this->warning_msg.set_text(this->warning_buffer);
}

namespace
{
    char const* consume_spaces(char const* s) noexcept
    {
        while (*s ==  ' ') {
            ++s;
        }
        return s;
    }

    // parse " *\d+h *\d+m| *\d+[hm]" or returns 0
    std::chrono::minutes check_duration(const char * duration)
    {
        auto chars_res = decimal_chars_to_int<unsigned>(consume_spaces(duration));
        if (chars_res.ec == std::errc()) {
            unsigned long minutes = 0;

            if (*chars_res.ptr == 'h') {
                minutes = chars_res.val * 60;
                duration = consume_spaces(chars_res.ptr + 1);

                if (*duration) {
                    chars_res = decimal_chars_to_int<unsigned>(duration);
                    if (chars_res.ec != std::errc()) {
                        return std::chrono::minutes(0);
                    }
                    duration = chars_res.ptr + 1;
                }
            }

            if (*chars_res.ptr == 'm') {
                minutes += chars_res.val;
                duration = chars_res.ptr + 1;
            }

            if (*duration == '\0') {
                return std::chrono::minutes(minutes);
            }
        }

        return std::chrono::minutes(0);
    }
} // anonymous namespace

void WidgetForm::check_confirmation()
{
    if (((this->flags & DURATION_DISPLAY) == DURATION_DISPLAY) &&
        ((this->flags & DURATION_MANDATORY) == DURATION_MANDATORY) &&
        (this->duration_edit.num_chars == 0)
    ) {
        this->set_warning_buffer(trkeys::fmt_field_required, tr(trkeys::duration));
        this->set_widget_focus(this->duration_edit, focus_reason_mousebutton1);
        this->rdp_input_invalidate(this->get_rect());
        return;
    }

    if (((this->flags & DURATION_DISPLAY) == DURATION_DISPLAY) &&
        (this->duration_edit.num_chars != 0)) {
        std::chrono::minutes res = check_duration(this->duration_edit.get_text());
        // res is duration in minutes.
        if (res <= 0min || res > this->duration_max) {
            if (res <= 0min) {
                this->duration_edit.set_text("");
                this->set_warning_buffer(trkeys::fmt_invalid_format, tr(trkeys::duration));
            }
            else {
                this->set_warning_buffer(trkeys::fmt_toohigh_duration, tr(trkeys::duration),
                    int(this->duration_max.count()));
            }
            this->set_widget_focus(this->duration_edit, focus_reason_mousebutton1);
            this->rdp_input_invalidate(this->get_rect());
            return;
        }
    }

    if (((this->flags & TICKET_DISPLAY) == TICKET_DISPLAY) &&
        ((this->flags & TICKET_MANDATORY) == TICKET_MANDATORY) &&
        (this->ticket_edit.num_chars == 0)
    ) {
        this->set_warning_buffer(trkeys::fmt_field_required, tr(trkeys::ticket));
        this->set_widget_focus(this->ticket_edit, focus_reason_mousebutton1);
        this->rdp_input_invalidate(this->get_rect());
        return;
    }

    if (((this->flags & COMMENT_DISPLAY) == COMMENT_DISPLAY) &&
        ((this->flags & COMMENT_MANDATORY) == COMMENT_MANDATORY) &&
        (this->comment_edit.num_chars == 0)
    ) {
        this->set_warning_buffer(trkeys::fmt_field_required, tr(trkeys::comment));
        this->set_widget_focus(this->comment_edit, focus_reason_mousebutton1);
        this->rdp_input_invalidate(this->get_rect());
        return;
    }

    this->events.submit();
}

void WidgetForm::rdp_input_scancode(KbdFlags flags, Scancode scancode, uint32_t event_time, Keymap const& keymap)
{
    if (pressed_scancode(flags, scancode) == Scancode::Esc) {
        this->events.cancel();
    }
    else {
        WidgetComposite::rdp_input_scancode(flags, scancode, event_time, keymap);
    }
}
