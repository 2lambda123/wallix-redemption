/*
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   Product name: redemption, a FLOSS RDP proxy
   Copyright (C) Wallix 2020
   Author(s): Christophe Grosjean, Jonathan Poelen

   Unit test to conversion of RDP drawing orders to PNG images
*/

#include "test_only/test_framework/redemption_unit_tests.hpp"
#include "test_only/test_framework/compare_collection.hpp"
#include "test_only/test_framework/check_img.hpp"

#include "gdi/text_metrics.hpp"
#include "utils/sugar/to_sv.hpp"
#include "test_only/core/font.hpp"
#include "test_only/gdi/test_graphic.hpp"

#if !REDEMPTION_UNIT_TEST_FAST_CHECK
#include <iomanip>
#endif

#define IMG_TEST_PATH FIXTURES_PATH "/img_ref/gdi/text_metrics/"


RED_AUTO_TEST_CASE(TextMetrics)
{
    {
        gdi::TextMetrics text(global_font_deja_vu_14(), "abc"_av);
        RED_CHECK_EQUAL(18, text.height);
        RED_CHECK_EQUAL(25, text.width);
    }
    {
        gdi::TextMetrics text(global_font_deja_vu_14(), "abcde"_av);
        RED_CHECK_EQUAL(18, text.height);
        RED_CHECK_EQUAL(43, text.width);
    }
    {
        gdi::TextMetrics text(global_font_deja_vu_14(), "Ay"_av);
        RED_CHECK_EQUAL(18, text.height);
        RED_CHECK_EQUAL(19, text.width);
    }
}


struct LineForTest : bytes_view
{
    std::string_view sv() const
    {
        return to_sv(as_chars());
    }

    bool operator == (LineForTest const& other) const
    {
        return sv() == other.sv();
    }
};

#if !REDEMPTION_UNIT_TEST_FAST_CHECK
static ut::assertion_result test_comp_lines(
    array_view<LineForTest> const& a,
    array_view<LineForTest> const& b)
{
    return ut::ops::compare_collection_EQ(a, b, [&](std::ostream& out, size_t pos, ...) /*NOLINT*/
    {
        auto put_view = [&](std::ostream& oss, auto av){
            if (av.empty()) {
                oss << "--";
            }
            else {
                size_t i = 0;
                auto put = [&](LineForTest const& line) {
                    oss << (i == pos ? "*{" : "{") << std::quoted(line.sv()) << "}";
                    ++i;
                };
                put(av.front());
                for (auto const& l : av.from_offset(1)) {
                    oss << " ";
                    put(l);
                }
            }
        };

        ut::put_data_with_diff(out, a, "!=", b, put_view);
    });
}

RED_TEST_DISPATCH_COMPARISON_EQ((), (::array_view<::LineForTest>), (::array_view<::LineForTest>), ::test_comp_lines)
#endif

#define TEST_LINES(font, s, max_width, ...) [&](                                          \
    gdi::MultiLineTextMetrics const& metrics                                              \
) {                                                                                       \
    bytes_view expected_[] {__VA_ARGS__};                                                 \
    array_view lines_ = metrics.lines();                                                  \
    array_view<LineForTest> expected{                                                     \
        static_cast<LineForTest const*>(&expected_[0]), std::size(expected_)}; /*NOLINT*/ \
    array_view lines = {                                                                  \
        static_cast<LineForTest const*>(lines_.data()), lines_.size()}; /*NOLINT*/        \
    RED_CHECK(lines == expected);                                                         \
}(gdi::MultiLineTextMetrics(font, s ""_av, max_width))


RED_AUTO_TEST_CASE(MultiLineTextMetrics)
{
    auto& font14 = global_font_deja_vu_14();

    RED_TEST(gdi::MultiLineTextMetrics(font14, ""_av, 0).lines().size() == 0);

    TEST_LINES(font14, "ab", 0,
        "a"_av,
        "b"_av,
    );

    TEST_LINES(font14, "abc", 100,
        "abc"_av,
    );

    TEST_LINES(font14, "abc", 17,
        "ab"_av,
        "c"_av,
    );

    TEST_LINES(font14, "abc", 1,
        "a"_av,
        "b"_av,
        "c"_av,
    );

    TEST_LINES(font14, "a\nb\nc", 100,
        "a"_av,
        "b"_av,
        "c"_av,
    );

    TEST_LINES(font14, "a\nb\nc", 17,
        "a"_av,
        "b"_av,
        "c"_av,
    );

    TEST_LINES(font14, "a\nb\nc", 1,
        "a"_av,
        "b"_av,
        "c"_av,
    );

    TEST_LINES(font14, "ab cd", 17,
        "ab"_av,
        "cd"_av,
    );

    TEST_LINES(font14, "ab   cd", 17,
        "ab"_av,
        "cd"_av,
    );

    TEST_LINES(font14, "ab cd", 1,
        "a"_av,
        "b"_av,
        "c"_av,
        "d"_av,
    );

    TEST_LINES(font14, "annvhg jgsy kfhdis hnvlkj gks hxk.hf", 50,
        "annvh"_av,
        "g jgsy"_av,
        "kfhdis"_av,
        "hnvlkj"_av,
        "gks"_av,
        "hxk.hf"_av,
    );

    TEST_LINES(font14, "annvhg jgsy kfhdis hnvlkj gks hxk.hf", 150,
        "annvhg jgsy kfhdis"_av,
        "hnvlkj gks hxk.hf"_av,
    );

    TEST_LINES(font14, "veryverylonglonglong string", 100,
        "veryverylongl"_av,
        "onglong"_av,
        "string"_av,
    );

    TEST_LINES(font14, "  veryverylonglonglong string", 100,
        ""_av,
        "veryverylongl"_av,
        "onglong"_av,
        "string"_av,
    );

    TEST_LINES(font14, "  veryverylonglonglong string", 130,
        ""_av,
        "veryverylonglongl"_av,
        "ong string"_av,
    );

    TEST_LINES(font14, "  veryverylonglonglong\n string", 100,
        ""_av,
        "veryverylongl"_av,
        "onglong"_av,
        " string"_av,
    );

    TEST_LINES(font14, "  veryverylonglonglong \nstring", 100,
        ""_av,
        "veryverylongl"_av,
        "onglong"_av,
        "string"_av,
    );

    TEST_LINES(font14, "bla bla\n\n - abc\n - def", 100,
        "bla bla"_av,
        ""_av,
        " - abc"_av,
        " - def"_av,
    );

    TEST_LINES(font14, "Le pastafarisme (mot-valise faisant référence aux pâtes et au mouvement rastafari) est originellement une parodie de religion1,2,3,4 dont la divinité est le Monstre en spaghetti volant (Flying Spaghetti Monster)5,6 créée en 2005 par Bobby Henderson, alors étudiant de l'université d'État de l'Oregon. Depuis, le pastafarisme a été reconnu administrativement comme religion par certains pays7,8,9,10,11, et rejeté en tant que telle par d'autres12,13,14.", 273,
        "Le pastafarisme (mot-valise faisant"_av,
        "référence aux pâtes et au"_av,
        "mouvement rastafari) est"_av,
        "originellement une parodie de"_av,
        "religion1,2,3,4 dont la divinité est le"_av,
        "Monstre en spaghetti volant (Flying"_av,
        "Spaghetti Monster)5,6 créée en 2005"_av,
        "par Bobby Henderson, alors étudiant"_av,
        "de l'université d'État de l'Oregon."_av,
        "Depuis, le pastafarisme a été reconnu"_av,
        "administrativement comme religion"_av,
        "par certains pays7,8,9,10,11, et rejeté"_av,
        "en tant que telle par"_av,
        "d'autres12,13,14."_av,
    );
}


RED_AUTO_TEST_CASE(TestServerDrawText)
{
    auto& font = global_font_deja_vu_14();

    const uint16_t w = 1800;
    const uint16_t h = 30;
    TestGraphic gd(w, h);

    auto text = ""
        "Unauthorized access to this system is forbidden and will be prosecuted"
        " by law. By accessing this system, you agree that your actions may be"
        " monitored if unauthorized usage is suspected."
        ""_av
    ;

    using color_encoder = encode_color24;
    gdi::server_draw_text(
        gd, font, 0, 0, text,
        color_encoder()(NamedBGRColor::CYAN),
        color_encoder()(NamedBGRColor::BLUE),
        gdi::ColorCtx::from_bpp(color_encoder::bpp, nullptr),
        Rect(0, 0, w, h));

    RED_CHECK_IMG(gd, IMG_TEST_PATH "server_draw_text1.png");
}
