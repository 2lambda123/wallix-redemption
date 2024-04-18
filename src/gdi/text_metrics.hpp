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

#include "gdi/graphic_api.hpp"
#include "utils/sugar/bytes_view.hpp"

#include <memory>

class Font;

namespace gdi
{

struct TextMetrics
{
    int width = 0;
    int height = 0;

    explicit TextMetrics(const Font & font, bytes_view utf8_text);

    static int char_width(const Font & font, uint32_t unicode);
};

struct MultiLineTextMetrics
{
    explicit MultiLineTextMetrics() noexcept = default;
    explicit MultiLineTextMetrics(const Font& font, bytes_view utf8_text, unsigned max_width);

    MultiLineTextMetrics(MultiLineTextMetrics const&) = delete;
    MultiLineTextMetrics operator = (MultiLineTextMetrics const&) = delete;

    MultiLineTextMetrics(MultiLineTextMetrics&& other) noexcept
        : d(other.d)
    {
        other.d = Data();
    }

    MultiLineTextMetrics& operator=(MultiLineTextMetrics&& other) noexcept
    {
        MultiLineTextMetrics g(std::move(*this));
        std::swap(d, other.d);
        return *this;
    }

    ~MultiLineTextMetrics();

    array_view<bytes_view> lines() const noexcept
    {
        return {d.lines, d.nb_line};
    }

    uint16_t max_width() const noexcept
    {
        return d.max_width;
    }

private:
    struct Data {
        bytes_view* lines = nullptr;
        unsigned nb_line = 0;
        uint16_t max_width = 0;
    };

    Data d;
};


// TODO implementation of the server_draw_text function below is a small subset of possibilities text can be packed (detecting duplicated strings). See MS-RDPEGDI 2.2.2.2.1.1.2.13 GlyphIndex (GLYPHINDEX_ORDER)
// TODO: is it still used ? If yes move it somewhere else. Method from internal mods ?
void server_draw_text(
    GraphicApi & drawable, Font const & font,
    int16_t x, int16_t y, bytes_view utf8_text,
    RDPColor fgcolor, RDPColor bgcolor,
    ColorCtx color_ctx,
    Rect clip
);

}  // namespace gdi
