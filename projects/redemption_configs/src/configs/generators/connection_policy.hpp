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
*   Copyright (C) Wallix 2010-2015
*   Author(s): Jonathan Poelen
*/

#pragma once

#include "configs/attributes/spec.hpp"
#include "configs/generators/utils/spec_writer.hpp"
#include "configs/generators/utils/write_template.hpp"
#include "configs/enumeration.hpp"

#include "configs/generators/python_spec.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>

#include <cerrno>
#include <cstring>


namespace cfg_generators {

namespace connection_policy_writer {

using namespace cfg_attributes;

template<class Inherit>
struct ConnectionPolicyWriterBase : python_spec_writer::PythonSpecWriterBase<Inherit, connpolicy::name>
{
    using base_type = ConnectionPolicyWriterBase;

    using filename_map_t = std::array<std::pair<std::string, char const*>, 2>;

    ConnectionPolicyWriterBase(filename_map_t filename_map, char const* sesman_map_filename)
    : python_spec_writer::PythonSpecWriterBase<Inherit, connpolicy::name>(sesman_map_filename)
    , filename_map(std::move(filename_map))
    {}

    // used by python_spec_writer
    static io_prefix_lines comment(char const * s)
    {
        return io_prefix_lines{s, "# ", "", 0};
    }

    template<class Pack>
    void do_member(std::string const & section_name, Pack const& infos)
    {
        if constexpr (is_convertible_v<Pack, connection_policy_t>) {
            auto type = get_type<sesman::type_>(infos);
            std::string const& member_name = get_name<connpolicy::name>(infos);

            this->write_description(type, infos);
            this->inherit().write_type_info(type);
            this->write_enumeration_value_description(type, infos);

            auto& connpolicy = get_elem<connection_policy_t>(infos);

            using attr1_t = spec::internal::attr;
            using attr2_t = connpolicy::internal::attr;
            auto attr1 = value_or<spec_attr_t>(infos, spec_attr_t{attr1_t::no_ini_no_gui}).value;
            auto attr2 = connpolicy.spec;

            if (bool(attr1 & attr1_t::advanced_in_gui)
             || bool(attr2 & attr2_t::advanced_in_connpolicy))
                this->out() << "#_advanced\n";
            if (bool(attr1 & attr1_t::hex_in_gui)
             || bool(attr2 & attr2_t::hex_in_connpolicy))
                this->out() << "#_hex\n";

            this->out() << member_name << " = ";
            this->inherit().write_type(type, get_default<connpolicy::default_>(type, infos));
            this->out() << "\n\n";

            auto const& section = value_or<connpolicy::section>(
                infos, connpolicy::section{section_name.c_str()});

            auto& sesman_name = get_name<sesman::name>(infos);

            auto s = this->out_member_.str();
            for (auto const& file : connpolicy.files) {
                auto&& sections = file_map[file];
                auto& sec = (section_name == section.name)
                    ? sections.section_map[section.name]
                    : sections.delayed_section_map[section.name];
                sec += s;
            }

            std::string sesman_mem_key = section.name;
            sesman_mem_key += '/';
            sesman_mem_key += sesman_name;
            sesman_mem_key += '/';
            sesman_mem_key += member_name;
            if (!this->sesman_mems.emplace(sesman_mem_key).second) {
                throw std::runtime_error(std::string("duplicate ") + section.name + ' ' + member_name);
            }

            this->ordered_sesman_sections.emplace(section.name);
            auto& file_content = this->sesman_file[section.name];
            update_sesman_contains(file_content, sesman_name, member_name);

            if constexpr (is_convertible_v<Pack, sesman::deprecated_names>) {
                for (auto&& old_name : get_elem<sesman::deprecated_names>(infos).names) {
                    update_sesman_contains(file_content, old_name, member_name, " # Deprecated, for compatibility only.");
                }
            }

            this->out_member_.str("");
        }
    }

    static void update_sesman_contains(
        std::string& s,
        std::string const sesman_name,
        std::string const connpolicy_name,
        char const* extra = "")
    {
        s += "    u'";
        s += sesman_name;
        s += "': '";
        s += connpolicy_name;
        s += "',";
        s += extra;
        s += '\n';
    }

    void do_init()
    {
    }

    void do_finish()
    {
        for (auto const& [cat, filename] : filename_map) {
            auto file_it = file_map.find(cat);
            if (file_it == file_map.end()) {
                this->errorstring += "Config: unknown ";
                this->errorstring += cat;
                this->errorstring += " for connpolicy";
                return ;
            }

            std::ofstream out_spec(filename);
            out_spec << R"g([general]

# Secondary login Transformation rule
# ${LOGIN} will be replaced by login
# ${DOMAIN} (optional) will be replaced by domain if it exists.
# Empty value means no transformation rule.
transformation_rule = string(default='')

# Account Mapping password retriever
# Transformation to apply to find the correct account.
# ${USER} will be replaced by the user's login.
# ${DOMAIN} will be replaced by the user's domain (in case of LDAP mapping).
# ${USER_DOMAIN} will be replaced by the user's login + "@" + user's domain (or just user's login if there's no domain).
# ${GROUP} will be replaced by the authorization's user group.
# ${DEVICE} will be replaced by the device's name.
# A regular expression is allowed to transform a variable, with the syntax: ${USER:/regex/replacement}, groups can be captured with parentheses and used with \1, \2, ...
# For example to replace leading "A" by "B" in the username: ${USER:/^A/B}
# Empty value means no transformation rule.
vault_transformation_rule = string(default='')


)g";

            auto& section_map = file_it->second.section_map;
            auto& delayed_section_map = file_it->second.delayed_section_map;

            for (auto const& [section_name, section] : section_map) {
                out_spec << "[" << section_name << "]\n\n" << section;
                auto it = delayed_section_map.find(section_name);
                if (it != delayed_section_map.end()) {
                    out_spec << it->second;
                    delayed_section_map.erase(it);
                }
            }

            for (auto const& [section_name, section] : delayed_section_map) {
                out_spec << "[" << section_name << "]\n\n" << section;
            }

            if (!out_spec) {
                this->errorstring = filename;
                return;
            }
            out_spec.close();
        }


        auto& out_sesman = this->out_file_;

        out_sesman <<
          "#!/usr/bin/python -O\n"
          "# -*- coding: utf-8 -*-\n\n"
          "cp_spec = {\n"
        ;

        for (auto const& section_name : this->ordered_sesman_sections) {
            auto section_it = sesman_file.find(section_name);
            out_sesman
                << "  '" << section_name << "': {\n"
                << section_it->second << "  },\n"
            ;
        }

        out_sesman << "}\n";
    }

private:
    using data_by_section_t = std::unordered_map<std::string, std::string>;
    struct File
    {
        data_by_section_t section_map;
        data_by_section_t delayed_section_map;
    };
    std::unordered_map<std::string, File> file_map;
    std::unordered_map<std::string, std::string> sesman_file;
    std::unordered_set<std::string> sesman_mems;
    std::set<std::string> ordered_sesman_sections;
    filename_map_t filename_map;

public:
    std::string errorstring;
};

}


template<class SpecWriter>
int app_write_connection_policy(int ac, char const * const * av)
{
    if (ac < 4) {
        std::cerr << av[0] << " sesman-map.py rdp.spec vnc.spec\n";
        return 1;
    }

    SpecWriter writer(typename SpecWriter::filename_map_t{
        std::pair{"rdp", av[2]},
        std::pair{"vnc", av[3]}
    }, av[1]);
    writer.evaluate();

    if (!writer.out_file_) {
        std::cerr << av[0] << ": " << av[1] << ": " << strerror(errno) << "\n";
        return 1;
    }
    if (!writer.errorstring.empty()) {
        std::cerr << av[0] << ": " << writer.errorstring << "\n";
        return 1;
    }
    return 0;
}

}
