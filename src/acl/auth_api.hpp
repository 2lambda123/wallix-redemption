/*
    This program is free software; you can redistribute it and/or modify it
     under the terms of the GNU General Public License as published by the
     Free Software Foundation; either version 2 of the License, or (at your
     option) any later version.

    This program is distributed in the hope that it will be useful, but
     WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
     Public License for more details.

    You should have received a copy of the GNU General Public License along
     with this program; if not, write to the Free Software Foundation, Inc.,
     675 Mass Ave, Cambridge, MA 02139, USA.

    Product name: redemption, a FLOSS RDP proxy
    Copyright (C) Wallix 2013
    Author(s): Christophe Grosjean, Raphael Zhou
*/

#ifndef _REDEMPTION_ACL_AUTH_API_HPP_
#define _REDEMPTION_ACL_AUTH_API_HPP_

class auth_api {
public:
    virtual ~auth_api() {}

    virtual void set_auth_channel_target(const char * target) = 0;
    //virtual void set_auth_channel_result(const char * result) = 0;

    virtual void report(const char * reason, const char * message) = 0;
};


class NullAuthentifier : public auth_api {
    virtual void set_auth_channel_target(const char * target) {}
    //virtual void set_auth_channel_result(const char * result) {}

    virtual void report(const char * reason, const char * message) {}
};


inline NullAuthentifier * get_null_authentifier() {
    static NullAuthentifier auth;

    return &auth;
}

#endif  // #ifndef _REDEMPTION_ACL_AUTH_API_HPP_
