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
 *   Author(s): Christophe Grosjean, Dominique Lafages, Jonathan Poelen,
 *              Meng Tan
 */

#include "mod/internal/widget/multiline.hpp"
#include "gdi/graphic_api.hpp"
#include "core/RDP/orders/RDPOrdersPrimaryOpaqueRect.hpp"
#include "core/font.hpp"


WidgetMultiLine::WidgetMultiLine(
    gdi::GraphicApi & drawable,
    Color fgcolor, Color bgcolor, Font const & font,
    int xtext, int ytext)
: Widget(drawable, Focusable::Yes)
, x_text(xtext)
, y_text(ytext)
, cy_text(0)
, bg_color(bgcolor)
, fg_color(fgcolor)
, font(font)
{}

WidgetMultiLine::WidgetMultiLine(
    gdi::GraphicApi & drawable,
    chars_view text, unsigned max_width,
    Color fgcolor, Color bgcolor, Font const & font,
    int xtext, int ytext)
: WidgetMultiLine(drawable, fgcolor, bgcolor, font, xtext, ytext)
{
    this->set_text(text, max_width);
}

void WidgetMultiLine::set_text(bytes_view text, unsigned max_width)
{
    this->set_text(gdi::MultiLineTextMetrics(this->font, text, max_width));
}

void WidgetMultiLine::set_text(gdi::MultiLineTextMetrics&& line_metrics)
{
    this->line_metrics = std::move(line_metrics);
    this->cy_text = this->font.max_height();
}

void WidgetMultiLine::rdp_input_invalidate(Rect clip)
{
    Rect rect_intersect = clip.intersect(this->get_rect());

    if (!rect_intersect.isempty()) {
        int dy = this->y();
        this->drawable.draw(
            RDPOpaqueRect(rect_intersect, this->bg_color),
            this->get_rect(), gdi::ColorCtx::depth24());
        for (auto const& line : this->line_metrics.lines()) {
            dy += this->y_text;
            gdi::server_draw_text(this->drawable
                                , this->font
                                , this->x_text + this->x()
                                , dy
                                , line
                                , this->fg_color
                                , this->bg_color
                                , gdi::ColorCtx::depth24()
                                , rect_intersect.intersect(
                                    Rect(this->x(), dy, this->cx(), this->cy_text))
                );
            dy += this->cy_text;
        }
    }
}

Dimension WidgetMultiLine::get_optimal_dim() const
{
    return Dimension(
        this->line_metrics.max_width() + this->x_text * 2,
        (this->cy_text + this->y_text) * this->line_metrics.lines().size()
    );
}
