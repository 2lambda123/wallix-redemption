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
   Copyright (C) Wallix 2010-2012
   Author(s): Christophe Grosjean, Javier Caverni, Raphael Zhou, Meng Tan
*/

#pragma once

#include "utils/sugar/unique_fd.hpp"
#include "utils/monotonic_clock.hpp"

class Inifile;
class PidFile;
class Font;

void session_start_tls(unique_fd sck,
                       MonotonicTimePoint sck_start_time,
                       Inifile& ini,
                       PidFile& pid_file,
                       Font const& font,
                       bool prevent_early_log);

void session_start_ws(unique_fd sck,
                      MonotonicTimePoint sck_start_time,
                      Inifile& ini,
                      PidFile& pid_file,
                      Font const& font,
                      bool prevent_early_log);

void session_start_wss(unique_fd sck,
                       MonotonicTimePoint sck_start_time,
                       Inifile& ini,
                       PidFile& pid_file,
                       Font const& font,
                       bool prevent_early_log);
