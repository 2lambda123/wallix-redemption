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
 *   Author(s): Christophe Grosjean, Meng Tan, Jennifer Inthavong
 */

#include "core/app_path.hpp"
#include "core/font.hpp"
#include "core/RDP/orders/RDPOrdersPrimaryOpaqueRect.hpp"
#include "gdi/graphic_api.hpp"
#include "keyboard/keymap.hpp"
#include "mod/internal/widget/login.hpp"
#include "mod/internal/widget/multiline_borders.hpp"
#include "utils/theme.hpp"
#include "utils/trkeys.hpp"


WidgetLogin::WidgetLogin(
    gdi::GraphicApi & drawable, CopyPaste & copy_paste,
    WidgetTooltipShower & tooltip_shower,
    int16_t left, int16_t top, uint16_t width, uint16_t height,
    Events events, chars_view caption,
    chars_view login, chars_view password, chars_view target,
    chars_view label_text_login,
    chars_view label_text_password,
    chars_view label_text_target,
    chars_view label_error_message,
    chars_view login_message,
    WidgetButton * extra_button,
    bool enable_target_field,
    Font const & font, Translator tr, Theme const & theme
)
    : WidgetComposite(drawable, Focusable::Yes)
    , oncancel(events.oncancel)
    , onctrl_shift(events.onctrl_shift)
    , tooltip_shower(tooltip_shower)
    , error_message_label(drawable, label_error_message,
                    theme.global.error_color, theme.global.bgcolor,
                    font)
    , login_label(drawable, label_text_login,
                    theme.global.fgcolor, theme.global.bgcolor, font)
    , login_edit(drawable, copy_paste, login, events.onsubmit,
                 theme.edit.fgcolor, theme.edit.bgcolor,
                 theme.edit.focus_color, theme.global.bgcolor, font,
                 label_text_login, (width <= 640), 1, 1, false)
    , password_label(drawable, label_text_password,
                     theme.global.fgcolor, theme.global.bgcolor,
                     font)
    , password_edit(drawable, copy_paste, password, events.onsubmit,
                    theme.edit.fgcolor,
                    theme.edit.bgcolor, theme.edit.focus_color, theme.global.bgcolor,
                    font, label_text_password, (width <= 640), 1, 1, true)
    , target_label(drawable, label_text_target,
                   theme.global.fgcolor, theme.global.bgcolor, font)
    , target_edit(drawable, copy_paste, target, events.onsubmit,
                  theme.edit.fgcolor, theme.edit.bgcolor,
                  theme.edit.focus_color, theme.global.bgcolor, font,
                  label_text_target, (width <= 640), 1, 1, false)
    , message_label(drawable,
        login_message,
        theme.global.fgcolor, theme.global.bgcolor, theme.global.focus_color,
        font, WIDGET_MULTILINE_BORDER_X, WIDGET_MULTILINE_BORDER_Y)
    , img(drawable,
          theme.enable_theme ? theme.logo_path.c_str() :
          app_path(AppPath::LoginWabBlue),
          theme.global.bgcolor)
    , version_label(drawable, caption,
                    theme.global.fgcolor, theme.global.bgcolor,
                    font)
    // TODO button without notifier
    , helpicon(drawable, "?"_av, WidgetEventNotifier(),
               theme.global.fgcolor, theme.global.bgcolor,
               theme.global.focus_color, 2, font, 6, 2)
    , extra_button(extra_button)
    , tr(tr)
    , show_target(enable_target_field)
{
    this->set_bg_color(theme.global.bgcolor);
    this->helpicon.set_unfocusable();

    this->add_widget(this->img);
    this->add_widget(this->helpicon);

    if (this->show_target) {
        this->add_widget(this->target_edit);
    }

    bool focus_on_login = this->login_edit.get_text().empty();
    this->add_widget(this->login_edit, focus_on_login ? HasFocus::Yes : HasFocus::No);
    this->add_widget(this->password_edit, focus_on_login ? HasFocus::No : HasFocus::Yes);

    this->add_widget(this->version_label);

    this->add_widget(this->error_message_label);

    if (!login_message.empty()) {
        this->add_widget(this->message_label);
    }

    if (extra_button) {
        this->add_widget(*extra_button);
    }

    this->move_size_widget(left, top, width, height);
}

void WidgetLogin::move_size_widget(int16_t left, int16_t top, uint16_t width, uint16_t height)
{
    this->set_xy(left, top);
    this->set_wh(width, height);

    auto set_optimal_wh = [](auto& widget){
        widget.set_wh(widget.get_optimal_dim());
    };

    if (width > 640) {
        if (!this->labels_added) {
            if (this->show_target) {
                this->add_widget(this->target_label);
            }
            this->add_widget(this->login_label);
            this->add_widget(this->password_label);

            this->labels_added = true;
        }

        set_optimal_wh(this->login_label);
        set_optimal_wh(this->target_label);
        set_optimal_wh(this->password_label);
    }
    else {
        if (this->labels_added) {
            if (this->show_target) {
                this->remove_widget(this->target_label);
            }
            this->remove_widget(this->login_label);
            this->remove_widget(this->password_label);

            this->labels_added = false;
        }

        this->target_label.set_wh(0, 0);
        this->login_label.set_wh(0, 0);
        this->password_label.set_wh(0, 0);
    }

    const Dimension edit_dim = {
        uint16_t((width >= 420) ? 400 : width - 20),
        this->login_edit.get_optimal_dim().h
    };

    this->target_edit.use_title(width < 640);
    this->target_edit.set_wh(edit_dim);

    this->login_edit.use_title(width < 640);
    this->login_edit.set_wh(edit_dim);

    this->password_edit.use_title(width < 640);
    this->password_edit.set_wh(edit_dim);

    this->error_message_label.set_wh(
        edit_dim.w,
        this->error_message_label.get_optimal_dim().h);

    set_optimal_wh(this->version_label);

    set_optimal_wh(this->helpicon);

    set_optimal_wh(this->img);

    const int labels_w = std::max({
        this->login_label.cx(),
        this->password_label.cx(),
        this->show_target ? this->target_label.cx() : uint16_t()
    }) + ((width > 640) ? 10 : 0);

    const int cbloc_w = labels_w + edit_dim.w;
    const int cbloc_x = (width - cbloc_w) / 2;

    const int original_space_h = 10;
    const int original_extra_space_between_label_h = 10;

    const int nb_label = this->show_target ? 5 : 4;
    const int min_message_h = edit_dim.h * 4;
    const int labels_h = edit_dim.h * nb_label;

    int space_h = 0;
    int extra_space_between_label_h = original_extra_space_between_label_h;
    int start_y = 0;

    int img_cy = this->img.cy();
    for (;;) {
        const int full_borders_h = (nb_label + 3) * original_space_h
                                 + (nb_label - 1) * extra_space_between_label_h;

        if (labels_h + min_message_h + full_borders_h + img_cy <= height) {
            const int total_message_info_border_h = original_space_h * 2
                                                  + extra_space_between_label_h / 2;
            const int bloc_h = labels_h + full_borders_h + img_cy;
            const int max_message_h = height - bloc_h;

            this->message_label.set_wh(cbloc_w, max_message_h);
            const Dimension message_dim {
                uint16_t(cbloc_w),
                this->message_label.get_optimal_dim().h
            };

            if (message_dim.h <= max_message_h) {
                this->message_label.set_wh(message_dim);
                int y = height / 2 - original_space_h - edit_dim.h - space_h;

                if (!(message_dim.h + total_message_info_border_h < y && height - y >= bloc_h)) {
                    y += edit_dim.h - space_h;
                }

                if (message_dim.h + total_message_info_border_h < y && height - y >= bloc_h) {
                    start_y = y;
                    this->message_label.set_wh(message_dim);
                    y = (y - message_dim.h - total_message_info_border_h) / 2 + original_space_h;
                    if (y + edit_dim.h - space_h + message_dim.h < start_y) {
                        y += edit_dim.h - space_h;
                    }
                    this->message_label.set_xy(left + cbloc_x, top + y);
                }
                else {
                    start_y = message_dim.h + total_message_info_border_h;
                    this->message_label.set_xy(left + cbloc_x, top + original_space_h);
                }
            }
            else {
                start_y = max_message_h + total_message_info_border_h;
                this->message_label.set_xy(left + cbloc_x, top + original_space_h);
            }

            space_h = original_space_h;
            break;
        }
        // no extra space space between widget
        else if (extra_space_between_label_h) {
            extra_space_between_label_h = 0;
        }
        // no space between widget
        else if (labels_h + min_message_h + img_cy <= height) {
            start_y = height - (labels_h + img_cy);
            this->message_label.set_wh(cbloc_w, start_y);
            this->message_label.set_xy(left + cbloc_x, top);
            break;
        }
        // image as background
        else if (img_cy) {
            img_cy = 0;
            extra_space_between_label_h = original_extra_space_between_label_h;
            this->img.set_xy(this->img.x(), top);
        }
        else {
            start_y = edit_dim.h * 3;
            this->message_label.set_wh(cbloc_w, start_y);
            this->message_label.set_xy(left + cbloc_x, top);
            break;
        }
    }

    int y = start_y + top;
    auto set_xy_label = [&](WidgetLabel& label){
        label.set_xy(left + cbloc_x, y);
        y += edit_dim.h + space_h;
    };

    auto set_xy_label_and_edit = [&](WidgetEditValid& edit, WidgetLabel& label) {
        set_xy_label(label);
        y += extra_space_between_label_h;
        edit.set_xy(left + cbloc_x + labels_w, label.y() - edit.get_border_height() - 3);
    };

    this->error_message_label.set_xy(left + cbloc_x + labels_w, y);
    y += edit_dim.h + space_h;
    if (this->show_target) {
        set_xy_label_and_edit(this->target_edit, this->target_label);
    }
    set_xy_label_and_edit(this->login_edit, this->login_label);
    set_xy_label_and_edit(this->password_edit, this->password_label);


    // Bottom bloc positioning
    // Logo and Version
    const int bottom_height = (height - y + top) / 2;
    const int bbloc_h       = (img_cy ? img_cy + space_h : 0) + this->version_label.cy();

    if (bbloc_h + 10 > bottom_height / 2) {
        y = top + height - (bbloc_h + space_h);
    }

    if (img_cy) {
        this->img.set_xy(left + (width - this->img.cx()) / 2, y);
        y += img_cy + space_h;
    }
    this->version_label.set_xy(left + (width - this->version_label.cx()) / 2, y);


    const auto bottom_w = img_cy
        ? std::max(this->version_label.cx(), this->img.cx())
        : this->version_label.cx();
    const int border_w = (width - bottom_w) / 2;
    const int helpicon_space_w = std::clamp((border_w - this->helpicon.cx()) / 2, 0, 60);
    const int space_w = this->extra_button
        ?  std::min(helpicon_space_w, std::clamp((border_w - this->extra_button->cx()) / 2, 0, 60))
        : helpicon_space_w;
    const int button_space_h = img_cy ? 30 : space_h;
    const int diff_button_h = this->extra_button
        ? (this->extra_button->cy() - this->helpicon.cy()) / 2
        : 0;
    this->helpicon.set_xy(
        left + width - (this->helpicon.cx() + space_w),
        top + height - (this->helpicon.cy() + button_space_h + diff_button_h)
    );

    if (this->extra_button) {
        this->extra_button->set_xy(
            left + space_w,
            top + height - button_space_h - this->extra_button->cy()
        );
    }
}

void WidgetLogin::rdp_input_scancode(KbdFlags flags, Scancode scancode, uint32_t event_time, Keymap const& keymap)
{
    REDEMPTION_DIAGNOSTIC_PUSH()
    REDEMPTION_DIAGNOSTIC_GCC_IGNORE("-Wswitch-enum")
    switch (keymap.last_kevent()){
        case Keymap::KEvent::Esc:
            this->oncancel();
            break;

        case Keymap::KEvent::PgUp:
            this->message_label.scroll_up();
            break;

        case Keymap::KEvent::PgDown:
            this->message_label.scroll_down();
            break;

        case Keymap::KEvent::Ctrl:
        case Keymap::KEvent::Shift:
            if (this->extra_button
                && keymap.is_shift_pressed()
                && keymap.is_ctrl_pressed()
            ) {
                this->onctrl_shift();
            }
            break;

        default:
            WidgetComposite::rdp_input_scancode(flags, scancode, event_time, keymap);
            break;
    }
    REDEMPTION_DIAGNOSTIC_POP()
}

void WidgetLogin::rdp_input_mouse(uint16_t device_flags, uint16_t x, uint16_t y)
{
    if (device_flags == MOUSE_FLAG_MOVE && this->helpicon.get_rect().contains_pt(x, y)) {
        this->tooltip_shower.show_tooltip(
            this->tr(trkeys::help_message),
            x, y, this->get_rect(), this->helpicon.get_rect()
        );
    }

    WidgetComposite::rdp_input_mouse(device_flags, x, y);
}
