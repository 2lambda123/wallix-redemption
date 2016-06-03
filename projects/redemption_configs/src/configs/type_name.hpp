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
*   Copyright (C) Wallix 2010-2016
*   Author(s): Jonathan Poelen
*/

#pragma once

#include <iosfwd>
#include <type_traits>

#include <cstddef>
#include <cstring>


struct string_type_name
{
    char const * first;
    char const * last;

    char const * begin() const noexcept { return this->first; }
    char const * end() const noexcept { return this->last; }
    char const * data() const noexcept { return this->first; }

    std::size_t size() const noexcept { return this->last - this->first; }

    template<class Ch, class Tr>
    friend std::basic_ostream<Ch, Tr> & operator<<(std::basic_ostream<Ch, Tr> & out, string_type_name const & s)
    { return out.write(s.data(), s.size()); }
};

namespace detail
{
    template<class T, bool = std::is_integral<T>::value || std::is_enum<T>::value>
    struct type_name_impl;

    template<class T>
    struct type_name_impl<T, true>
    {
        static string_type_name impl()
        {
            char const * s = __PRETTY_FUNCTION__;
#ifdef __clang__
            return {s + 70, s + strlen(s) - 1};
#elif defined(__GNUG__)
            return {s + 74, s + strlen(s) - 1};
#endif
        }
    };
}

template<class T>
string_type_name type_name(T const * = nullptr)
{ return detail::type_name_impl<T>::impl(); }

#define CONFIG_DEFINE_TYPE_NAME(type, name)                      \
    namespace detail {                                    \
        template<bool B> struct type_name_impl<type, B> { \
            static string_type_name impl() {              \
                return {name, name + sizeof(name) - 1};   \
            }                                             \
        };                                                \
    }

#define CONFIG_DEFINE_TYPE_NAME2(type) \
    CONFIG_DEFINE_TYPE_NAME(type, #type)

#define CONFIG_DEFINE_TYPE(type) \
    class type {};        \
    CONFIG_DEFINE_TYPE_NAME(type, #type)

CONFIG_DEFINE_TYPE_NAME2(std::string)
