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
 *   Author(s): Christophe Grosjean, Meng Tan
 */

#pragma once

#include "utils/colors.hpp"

#include <string>


struct Theme
{
    struct {
        bool enable_theme = false;
        BGRColor bgcolor = NamedBGRColor::DARK_BLUE_BIS;
        BGRColor fgcolor = NamedBGRColor::WHITE;
        BGRColor separator_color = NamedBGRColor::LIGHT_BLUE;
        BGRColor focus_color = NamedBGRColor::WINBLUE;
        BGRColor error_color = NamedBGRColor::YELLOW;
        std::string logo_path;
    } global;

    struct {
        BGRColor bgcolor = NamedBGRColor::WHITE;
        BGRColor fgcolor = NamedBGRColor::BLACK;
        BGRColor focus_color = NamedBGRColor::WINBLUE;
    } edit;

    struct {
        BGRColor bgcolor = NamedBGRColor::LIGHT_YELLOW;
        BGRColor fgcolor = NamedBGRColor::BLACK;
        BGRColor border_color = NamedBGRColor::BLACK;
    } tooltip;

    struct {
        BGRColor bgcolor = NamedBGRColor::PALE_BLUE;
        BGRColor fgcolor = NamedBGRColor::BLACK;
    } selector_line1;
    struct {
        BGRColor bgcolor = NamedBGRColor::LIGHT_BLUE;
        BGRColor fgcolor = NamedBGRColor::BLACK;
    } selector_line2;
    struct {
        BGRColor bgcolor = NamedBGRColor::MEDIUM_BLUE;
        BGRColor fgcolor = NamedBGRColor::WHITE;
    } selector_selected;
    struct {
        BGRColor bgcolor = NamedBGRColor::WINBLUE;
        BGRColor fgcolor = NamedBGRColor::WHITE;
    } selector_focus;
    struct {
        BGRColor bgcolor = NamedBGRColor::MEDIUM_BLUE;
        BGRColor fgcolor = NamedBGRColor::WHITE;
    } selector_label;
};
