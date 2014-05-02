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
 *   Author(s): Christophe Grosjean, Meng Tan
 */

#if !defined(REDEMPTION_MOD_INTERNAL_WIDGET2_FLAT_LOGIN_HPP)
#define REDEMPTION_MOD_INTERNAL_WIDGET2_FLAT_LOGIN_HPP

#include "edit.hpp"
#include "edit_valid.hpp"
#include "label.hpp"
#include "password.hpp"
#include "multiline.hpp"
#include "image.hpp"
#include "composite.hpp"
#include "flat_button.hpp"
#include "translation.hpp"
#include "ellipse.hpp"
#include "theme.hpp"

// #include "scroll.hpp"

class FlatLogin : public WidgetParent
{
public:
    Theme & theme;
    WidgetEditValid  password_edit;
    WidgetLabel login_label;
    WidgetEditValid  login_edit;
    WidgetImage img;
    WidgetLabel password_label;
    WidgetLabel version_label;

    WidgetFlatButton helpicon;
    Inifile & ini;

    CompositeTable composite_table;

    // WidgetFrame frame;
    // WidgetImage wimage;
    // WidgetVScrollBar bar;


    FlatLogin(DrawApi& drawable, uint16_t width, uint16_t height, Widget2 & parent,
              NotifyApi* notifier, const char* caption,
              bool focus_on_password, int group_id,
              const char * login, const char * password,
              const char * label_text_login,
              const char * label_text_password,
              Inifile & ini)
        : WidgetParent(drawable, Rect(0, 0, width, height), parent, notifier)
        , theme(ini.theme)
        , password_edit(drawable, 0, 0, (width >= 420) ? 400 : width - 20, *this, this,
                        password, -14, this->theme.edit.fgcolor,
                        this->theme.edit.bgcolor, this->theme.edit.focus_color,
                        -1u, 1, 1, true, (width <= 640) ? label_text_password : NULL)
        , login_label(drawable, 0, 0, *this, NULL, label_text_login, true, -11,
                      this->theme.global.fgcolor, this->theme.global.bgcolor)
        , login_edit(drawable, 0, 0, (width >= 420) ? 400 : width - 20, *this, this,
                     login, -12, this->theme.edit.fgcolor, this->theme.edit.bgcolor,
                     this->theme.edit.focus_color, -1u, 1, 1, false,
                     (width <= 640) ? label_text_login : NULL)
        // , img(drawable, 0, 0, ini.theme.global.logo_path, *this, NULL, -10)
        , img(drawable, 0, 0,
              theme.global.logo ? theme.global.logo_path :
              SHARE_PATH "/" LOGIN_WAB_BLUE, *this, NULL, -10)
        , password_label(drawable, 0, 0, *this, NULL, label_text_password, true, -13,
                         this->theme.global.fgcolor, this->theme.global.bgcolor)
        , version_label(drawable, 0, 0, *this, NULL, caption, true, -15,
                        this->theme.global.fgcolor, this->theme.global.bgcolor)
        , helpicon(drawable, 0, 0, *this, NULL, "?", true, -16,
                   this->theme.global.fgcolor, this->theme.global.bgcolor,
                   this->theme.global.focus_color, 6, 2)
        , ini(ini)
        // , frame(drawable, Rect(0, 0, 600, 250), parent, notifier, -17)
        // , wimage(drawable, 0, 0, SHARE_PATH "/Philips_PM5544_640.bmp",
        //          parent, notifier, -17)
        // , bar(drawable, parent, notifier, MEDIUM_BLUE, LIGHT_BLUE, -17)
    {
        this->impl = &composite_table;
        this->add_widget(&this->helpicon);
        this->add_widget(&this->img);
        this->add_widget(&this->login_edit);
        this->add_widget(&this->password_edit);
        if (width > 640) {
            this->add_widget(&this->login_label);
            this->add_widget(&this->password_label);
        }
        else {
            this->password_label.rect.cx = 0;
            this->login_label.rect.cx = 0;
        }
        this->add_widget(&this->version_label);


        // Center bloc positionning
        // Login and Password boxes
        int cbloc_w = std::max(this->password_label.rect.cx + this->password_edit.rect.cx + 10,
                               this->login_label.rect.cx + this->login_edit.rect.cx + 10);
        int cbloc_h = std::max(this->password_label.rect.cy + this->login_label.rect.cy + 20,
                               this->password_edit.rect.cy + this->login_edit.rect.cy + 20);

        int x_cbloc = (width  - cbloc_w) / 2;
        int y_cbloc = (height - cbloc_h) / 2;

        this->login_label.set_xy(x_cbloc, y_cbloc);
        this->password_label.set_xy(x_cbloc, height/2);
        int labels_w = std::max(this->password_label.rect.cx, this->login_label.rect.cx);
        this->login_edit.set_xy(x_cbloc + labels_w + 10, y_cbloc);
        this->password_edit.set_xy(x_cbloc + labels_w + 10, height/2);

        this->login_label.rect.y += (this->login_edit.cy() - this->login_label.cy()) / 2;
        this->password_label.rect.y += (this->password_edit.cy() - this->password_label.cy()) / 2;


        // Bottom bloc positioning
        // Logo and Version
        int bottom_height = (height - cbloc_h) / 2;
        int bbloc_h = this->img.rect.cy + 10 + this->version_label.rect.cy;
        int y_bbloc = ((bbloc_h + 10) > (bottom_height / 2))
            ?(height - (bbloc_h + 10))
            :(height/2 + cbloc_h/2 + bottom_height/2);
        this->img.set_xy((width - this->img.rect.cx) / 2, y_bbloc);
        this->version_label.set_xy((width - this->version_label.rect.cx) / 2,
                                   y_bbloc + this->img.rect.cy + 10);

        this->helpicon.tab_flag = IGNORE_TAB;
        this->helpicon.focus_flag = IGNORE_FOCUS;
        this->helpicon.set_button_x(width - 60);
        this->helpicon.set_button_y(height - 60);

        // this->add_widget(&this->frame);
        // this->add_widget(&this->bar);
        // this->frame.set_widget(&this->wimage);
        // this->bar.set_frame(&this->frame);
    }

    virtual ~FlatLogin()
    {
        this->clear();
    }

    virtual void draw(const Rect& clip)
    {
        this->impl->draw(clip);
        this->draw_inner_free(clip.intersect(this->rect), this->theme.global.bgcolor);

    }

    virtual void draw_inner_free(const Rect& clip, int bg_color) {
        Region region;
        region.rects.push_back(clip);

        this->impl->draw_inner_free(clip, bg_color, region);

        for (std::size_t i = 0, size = region.rects.size(); i < size; ++i) {
            this->drawable.draw(RDPOpaqueRect(region.rects[i], bg_color), region.rects[i]);
        }
    }

    virtual void notify(Widget2* widget, NotifyApi::notify_event_t event)
    {
        if ((widget == &this->login_edit
             || widget == &this->password_edit)
             && event == NOTIFY_SUBMIT) {
            this->send_notify(NOTIFY_SUBMIT);
        }
    }

    virtual void rdp_input_scancode(long int param1, long int param2, long int param3, long int param4, Keymap2* keymap)
    {
        if (keymap->nb_kevent_available() > 0){
            switch (keymap->top_kevent()){
            case Keymap2::KEVENT_ESC:
                keymap->get_kevent();
                this->send_notify(NOTIFY_CANCEL);
                break;
            default:
                WidgetParent::rdp_input_scancode(param1, param2, param3, param4, keymap);
                break;
            }
        }
    }

    virtual void rdp_input_mouse(int device_flags, int x, int y, Keymap2* keymap) {

        if (device_flags == MOUSE_FLAG_MOVE) {
            Widget2 * wid = this->widget_at_pos(x, y);
            if (wid == &this->helpicon) {
                this->show_tooltip(wid, TR("help_message", this->ini), x, y);
            }
        }

        WidgetParent::rdp_input_mouse(device_flags, x, y, keymap);
    }
};

#endif
