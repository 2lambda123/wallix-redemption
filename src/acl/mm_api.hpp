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
*   Copyright (C) Wallix 2010-2014
*   Author(s): Jonathan Poelen
*/

#ifndef REDEMPTION_ACL_MM_API_HPP
#define REDEMPTION_ACL_MM_API_HPP

#include "mod/mod_api.hpp"

class auth_api;

class MMApi
{
public:

    mod_api * mod;

    bool last_module;
    bool connected;

    MMApi() : mod(nullptr)
        , last_module(false)
        , connected(false) {}
    virtual ~MMApi() {}
    virtual void remove_mod() = 0;
    virtual void new_mod(int target_module, time_t now, auth_api * acl) = 0;
    virtual int next_module() = 0;
    // virtual int get_mod_from_protocol() = 0;
    virtual void invoke_close_box(const char * auth_error_message,
                                  BackEvent_t & signal, time_t now) {
        this->last_module = true;
    };
    //virtual bool is_last_module() {
    //    return this->last_module;
    //}
    virtual bool is_connected() {
        return this->connected;
    }
    virtual bool is_up_and_running() {
        bool res = false;
        if (this->mod) {
            res = this->mod->is_up_and_running();
        }
        return res;
    }
    virtual void record(auth_api * acl) {}
    virtual void stop_record() {}
    virtual void check_module() { }
};

#endif
