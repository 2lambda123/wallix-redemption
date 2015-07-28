/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Product name: redemption, a FLOSS RDP proxy
 *   Copyright (C) Wallix 2010-2013
 *   Author(s): Christophe Grosjean, Xiaopeng Zhou, Jonathan Poelen,
 *              Meng Tan
 */

#ifndef REDEMPTION_MOD_INTERNAL_FLAT_DIALOG_MOD_HPP
#define REDEMPTION_MOD_INTERNAL_FLAT_DIALOG_MOD_HPP

#include "translation.hpp"
#include "front_api.hpp"
#include "config.hpp"
#include "widget2/flat_dialog.hpp"
#include "widget2/screen.hpp"
#include "internal_mod.hpp"
#include "timeout.hpp"

class FlatDialogMod : public InternalMod, public NotifyApi
{
    FlatDialog dialog_widget;

    Inifile          & ini;
    TimeoutT<time_t>   timeout;

public:
    FlatDialogMod(Inifile & ini, FrontAPI & front, uint16_t width, uint16_t height,
                  const char * caption, const char * message, const char * cancel_text,
                  time_t now, ChallengeOpt has_challenge = NO_CHALLENGE)
        : InternalMod(front, width, height, ini.font, &ini)
        , dialog_widget(*this, width, height, this->screen, this, caption, message,
                        0, ini.theme, ini.font, TR("OK", ini), cancel_text,
                        has_challenge)
        , ini(ini)
        , timeout(now, ini.get<cfg::debug::pass_dialog_box>())
    {
        this->screen.add_widget(&this->dialog_widget);
        this->dialog_widget.set_widget_focus(&this->dialog_widget.ok, Widget2::focus_reason_tabkey);
        this->screen.set_widget_focus(&this->dialog_widget, Widget2::focus_reason_tabkey);
        this->screen.refresh(this->screen.rect);

        if (this->dialog_widget.challenge) {
            this->dialog_widget.set_widget_focus(this->dialog_widget.challenge, Widget2::focus_reason_tabkey);
            // this->ini.get<cfg::to_send_set::insert>()(AUTHID_AUTHENTICATION_CHALLENGE);
        }
    }

    ~FlatDialogMod() override {
        this->screen.clear();
    }

    void notify(Widget2* sender, notify_event_t event) override {
        switch (event) {
            case NOTIFY_SUBMIT: this->accepted(); break;
            case NOTIFY_CANCEL: this->refused(); break;
            default: ;
        }
    }

private:
    TODO("ugly. The value should be pulled by authentifier when module is closed instead of being pushed to it by mod")
    void accepted()
    {
        if (this->dialog_widget.challenge) {
            this->ini.context_set_value(AUTHID_PASSWORD,
                                        this->dialog_widget.challenge->get_text());
        }
        else {
            this->ini.context_set_value((this->dialog_widget.cancel
                                         ? AUTHID_ACCEPT_MESSAGE
                                         : AUTHID_DISPLAY_MESSAGE),
                                        "True");
        }
        this->event.signal = BACK_EVENT_NEXT;
        this->event.set();
    }

    TODO("ugly. The value should be pulled by authentifier when module is closed instead of being pushed to it by mod")
    void refused()
    {
        if (!this->dialog_widget.challenge) {
            this->ini.context_set_value((this->dialog_widget.cancel
                                         ? AUTHID_ACCEPT_MESSAGE
                                         : AUTHID_DISPLAY_MESSAGE),
                                        "False");
        }
        this->event.signal = BACK_EVENT_NEXT;
        this->event.set();
    }

public:
    void draw_event(time_t now) override {
        switch(this->timeout.check(now)) {
        case TimeoutT<time_t>::TIMEOUT_REACHED:
            this->accepted();
            break;
        case TimeoutT<time_t>::TIMEOUT_NOT_REACHED:
            this->event.set(1000000);
            break;
        default:
            this->event.reset();
            break;
        }
    }

};

#endif
