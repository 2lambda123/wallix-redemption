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
 *   Copyright (C) Wallix 2010-2012
 *   Author(s): Christophe Grosjean, Dominique Lafages, Jonathan Poelen,
 *              Meng Tan
 */

#include "test_only/test_framework/redemption_unit_tests.hpp"
#include "test_only/test_framework/check_img.hpp"
#include "test_only/test_framework/file.hpp"
#include "test_only/test_framework/working_directory.hpp"
#include "test_only/gdi/test_graphic.hpp"
#include "test_only/core/font.hpp"
#include "test_only/mod/internal/widget/null_tooltip_shower.hpp"

#include "mod/internal/widget/grid.hpp"
#include "mod/internal/widget/labelgrid.hpp"


#define IMG_TEST_PATH FIXTURES_PATH "/img_ref/mod/internal/widget/labelgrid/"

RED_AUTO_TEST_CASE(TraceLabelGrid)
{
    TestGraphic drawable(800, 600);
    NullTooltipShower tooltip_shower;

    int16_t x = 10;
    int16_t y = 10;

    const uint16_t line_number   = 5;
    const uint16_t column_number = 4;
    const uint16_t grid_border   = 2;

    WidgetLabelGrid wgrid(drawable, tooltip_shower, WidgetEventNotifier(),
                          line_number, column_number,
                          NamedBGRColor::PALE_BLUE, NamedBGRColor::BLACK,
                          NamedBGRColor::LIGHT_BLUE, NamedBGRColor::BLACK,
                          NamedBGRColor::WINBLUE, NamedBGRColor::WHITE,
                          NamedBGRColor::MEDIUM_BLUE, NamedBGRColor::WHITE,
                          global_font_deja_vu_14(), grid_border);
    wgrid.set_wh(640, 480);
    wgrid.set_xy(x, y);

    chars_view const texts0[] = { "target_group"_av, "target"_av, "protocol"_av, "timeframe"_av };
    wgrid.add_line(texts0);
    chars_view const texts1[] = { "win"_av, "admin@device"_av, "RDP"_av, "never"_av };
    wgrid.add_line(texts1);
    wgrid.add_line(texts1);
    wgrid.add_line(texts1);
    wgrid.set_selection(2);

    ColumnWidthStrategy column_width_strategies[] = {
        { 50, 150 }, { 150, 800 }, { 50, 150 }, { 50, 100 }
    };

    uint16_t row_height[line_number+4]              = { 0 };
    uint16_t column_width[column_number]            = { 0 };
    bool     column_width_is_optimal[column_number] = { false };

    compute_format(wgrid, column_width_strategies, -1, row_height, column_width, column_width_is_optimal);
    apply_format(wgrid, row_height, column_width);


    // ask to widget to redraw at it's current position
    wgrid.rdp_input_invalidate(wgrid.get_rect());

    RED_CHECK_IMG(drawable, IMG_TEST_PATH "label_grid_1.png");

    wgrid.has_focus = true;
    // ask to widget to redraw at it's current position
    wgrid.rdp_input_invalidate(wgrid.get_rect());

    RED_CHECK_IMG(drawable, IMG_TEST_PATH "label_grid_2.png");
}
