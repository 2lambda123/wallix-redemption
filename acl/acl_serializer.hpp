/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

  Product name: redemption, a FLOSS RDP proxy
  Copyright (C) Wallix 2010
  Author(s): Christophe Grosjean, Meng Tan

  Protocol layer for communication with ACL
  Updating context dictionnary from incoming acl traffic
*/

#ifndef _REDEMPTION_ACL_SERIALIZER_HPP_
#define _REDEMPTION_ACL_SERIALIZER_HPP_
#include <unistd.h>
#include <fcntl.h>


#include "stream.hpp"
#include "config.hpp"
#include "netutils.hpp"
#include "transport.hpp"
#include "translation.hpp"
#include "get_printable_password.hpp"

class AclSerializer{
    enum {
        HEADER_SIZE = 4
    };

    Inifile * ini;
    Transport & auth_trans;
    uint32_t verbose;

public:
    AclSerializer(Inifile * ini, Transport & auth_trans, uint32_t verbose)
        : ini(ini)
        , auth_trans(auth_trans)
        , verbose(verbose)
    {
        if (this->verbose & 0x10){
            LOG(LOG_INFO, "auth::AclSerializer");
        }
    }

    ~AclSerializer()
    {
        this->auth_trans.disconnect();
        if (this->verbose & 0x10){
            LOG(LOG_INFO, "auth::~AclSerializer");
        }
    }

    void in_items(Stream & stream)
    {
        if (this->verbose & 0x40){
            LOG(LOG_INFO, "auth::in_items");
        }
        for (; stream.p < stream.end ; this->in_item(stream)){
            ;
        }
    }

    void in_item(Stream & stream)
    {
        const char * keyword = reinterpret_cast<const char*>(stream.p);
        const uint8_t * start = stream.p;
        for ( ; stream.p < stream.end; ++stream.p){
            if (*stream.p == '\n') {
                *stream.p = 0;
                ++stream.p;
                break;
            }
        }
        const char * value = reinterpret_cast<const char*>(stream.p);
        for ( ; stream.p < stream.end; ++stream.p){
            if (*stream.p == '\n') {
                *stream.p = 0;

                if ((0 == strncasecmp(value, "ask", 3))) {
                    this->ini->ask_from_acl(keyword);
                    LOG(LOG_INFO, "receiving %s '%s'", value, keyword);
                }
                else {
                    // BASE64 TRY
                    // unsigned char output[32000];
                    // if (value[0] == '!') value++;
                    // size_t value_len = strlen((const char*)value);
                    // size_t out_len = this->ini->b64.decode(output, sizeof(output), (const unsigned char *)value, value_len);
                    // output[out_len] = 0;
                    // this->ini->set_from_acl((char *)keyword,
                    //                         (char *)output);
                    this->ini->set_from_acl(keyword, value + (value[0] == '!' ? 1 : 0));
                    const char * val         = this->ini->context_get_value_by_string(keyword);
                    const char * display_val = val;
                    if ((strncasecmp("password", keyword, 9 ) == 0) ||
                        (strncasecmp("target_password", keyword, 16) == 0) ||
                        ((strncasecmp("auth_channel_answer", keyword, 19) == 0) && (strcasestr(val, "password") != 0))) {
                        display_val = ::get_printable_password(val, this->ini->debug.password);
                    }
                    LOG(LOG_INFO, "receiving '%s'='%s'", keyword, display_val);
                }

                stream.p = stream.p+1;
                return;
            }
        }
        LOG(LOG_WARNING, "Unexpected exit while parsing ACL message");
        hexdump(start, stream.p-start);
        throw Error(ERR_ACL_UNEXPECTED_IN_ITEM_OUT);
    }

    void incoming()
    {
        BStream stream(HEADER_SIZE);
        this->auth_trans.recv(&stream.end, HEADER_SIZE);

        size_t size = stream.in_uint32_be();

        if (size > 65536){
            LOG(LOG_WARNING, "Error: ACL message too big (got %u max 64 K)", size);
            throw Error(ERR_ACL_MESSAGE_TOO_BIG);
        }
        if (size > stream.endroom()) {
            stream.init(size);
        }

        this->auth_trans.recv(&stream.end, size);

        if (this->verbose & 0x40){
            LOG(LOG_INFO, "ACL SERIALIZER : Data size without header (receive) = %u", size);
        }
        bool flag = this->ini->context.session_id.get().is_empty();
        this->in_items(stream);
        if (flag && !this->ini->context.session_id.get().is_empty()) {
            int child_pid = getpid();
            char old_session_file[256];
            sprintf(old_session_file, "%s/redemption/session_%d.pid", PID_PATH, child_pid);
            char new_session_file[256];
            sprintf(new_session_file, "%s/redemption/session_%s.pid", PID_PATH,
                    this->ini->context.session_id.get_cstr());
            rename(old_session_file, new_session_file);
        }
        if (this->verbose & 0x40){
            LOG(LOG_INFO, "SESSION_ID = %s", this->ini->context.session_id.get_cstr());
        }
    }

    TODO("move that function to Inifile create specialized stream object InifileStream "
         "maybe out_item should be in config , not here")
    void out_item(Stream & stream, authid_t authid)
    {
        const char * key = string_from_authid(authid);
        if (this->ini->context_is_asked(authid)){
            LOG(LOG_INFO, "sending (from authid) %s=ASK", key);
            stream.out_copy_bytes(key, strlen(key));
            stream.out_copy_bytes("\nASK\n",5);
        }
        else {
            const char * val         = this->ini->context_get_value(authid);
            const char * display_val = val;

            if ((strncasecmp("password", static_cast<const char*>(key), 8) == 0)
                ||(strncasecmp("target_password", static_cast<const char*>(key), 15) == 0)){
                display_val = ::get_printable_password(val, this->ini->debug.password);
            }
            LOG(LOG_INFO, "sending (from authid) %s=%s", key, display_val);
            stream.out_copy_bytes(key, strlen(key));
            stream.out_uint8('\n');
            stream.out_uint8('!');
            stream.out_copy_bytes(val, strlen(val));
            stream.out_uint8('\n');
        }
    }
    void out_item_new(Stream & stream, Inifile::BaseField * bfield)
    {
        char tmp[65536];
        const char * serialized = bfield->get_serialized(tmp, sizeof(tmp), this->ini->debug.password);
        bfield->use();
        stream.out_copy_bytes(serialized,strlen(serialized));
    }

    //void send_new(std::set<Inifile::BaseField *>& list)
    void send(Inifile::SetField const & list)
    {
        try {
            BStream stream(8192);
            stream.out_uint32_be(0);

            Inifile::SetField(list).foreach([&stream, this](Inifile::BaseField * bfield) {
                this->out_item_new(stream, bfield);
            });

            stream.mark_end();
            int total_length = stream.get_offset();
            if (this->verbose & 0x40){
                LOG(LOG_INFO, "ACL SERIALIZER : Data size without header (send) %u", total_length - HEADER_SIZE);
            }
            stream.set_out_uint32_be(total_length - HEADER_SIZE, 0); /* size in header */
            this->auth_trans.send(stream.get_data(), total_length);
        } catch (Error const &) {
            this->ini->context.authenticated.set(false);
            this->ini->context.rejected.set_from_cstr(TR("acl_fail", *(this->ini)));
            // this->ini->context.rejected.set_from_cstr("Authentifier service failed");
        }
    }

    void send_acl_data() {
        const Inifile::SetField & list = this->ini->get_changed_set();
        if (this->verbose & 0x01){
            LOG(LOG_INFO, "Begin Sending data to ACL: numbers of changed fields = %u",list.size());
        }
        if (!list.empty())
            this->send(list);
        this->ini->reset();
    }
};

#endif
