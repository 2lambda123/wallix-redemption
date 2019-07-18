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
*1324
*   Product name: redemption, a FLOSS VNC proxy
*   Copyright (C) Wallix 2018
*   Author(s): Clément Moroldo
*/

#include "test_only/test_framework/redemption_unit_tests.hpp"
#include "test_only/test_framework/file.hpp"

#include "utils/fileutils.hpp"
#include "test_only/test_framework/working_directory.hpp"

#include "mod/vnc/vnc_metrics.hpp"
#include "mod/metrics_hmac.hpp"


using namespace std::literals::chrono_literals;

RED_AUTO_TEST_CASE(TestVNCMetricsLogCycle1)
{
    WorkingDirectory wd("metrics_log_cycle1");

    auto epoch = to_timeval(1533211681s);

    Metrics m( wd.dirname()
              , "164d89c1a56957b752540093e178"
              , "51614130003BD5522C94E637866E4D749DDA13706AC2610C6F77BBFE111F3A58"_av
              , "1C57BA616EEDA5C9D8FF2E0202BB087D0B5D865AC830F336CDB9804331095B31"_av
              , "EAF28B142E03FFC03A35676722BB99DBC21908F3CEA96A8DA6E3C2321056AC48"_av
              , "B079C9845904075BAC3DBE0A26CB7364CE0CC0A5F47DC082F44D221EBC6722B7"_av
              , epoch
              , 24h
              , 5s
              );
    VNCMetrics metrics(m);

    auto const logmetrics1 = wd.add_file("vnc_metrics-v1.0-2018-08-02.logmetrics");
    auto const logindex1 = wd.add_file("vnc_metrics-v1.0-2018-08-02.logindex");
    RED_CHECK_WORKSPACE(wd);

    std::string expected_log_index("2018-08-02 12:08:01 connection 164d89c1a56957b752540093e178 user=51614130003BD5522C94E637866E4D749DDA13706AC2610C6F77BBFE111F3A58 account=1C57BA616EEDA5C9D8FF2E0202BB087D0B5D865AC830F336CDB9804331095B31 target_service_device=EAF28B142E03FFC03A35676722BB99DBC21908F3CEA96A8DA6E3C2321056AC48 client_info=B079C9845904075BAC3DBE0A26CB7364CE0CC0A5F47DC082F44D221EBC6722B7\n");

    RED_CHECK_FILE_CONTENTS(logmetrics1, ""_av);
    RED_CHECK_FILE_CONTENTS(logindex1, expected_log_index);

    {
        metrics.right_click();
        m.log(epoch);
        RED_CHECK_FILE_CONTENTS(logmetrics1, ""_av);
    }
    {
        m.log(epoch+1s);
        metrics.right_click();
        RED_CHECK_FILE_CONTENTS(logmetrics1, ""_av);
    }
    {
        m.log(epoch+3s);
        RED_CHECK_FILE_CONTENTS(logmetrics1, ""_av);
    }
    {
        std::string expected_log_metrics("2018-08-02 12:08:06 164d89c1a56957b752540093e178 0 0 0 0 0 0 2 0\n");
        m.log(epoch+5s);
        RED_CHECK_FILE_CONTENTS(logmetrics1, expected_log_metrics);;
    }
    {
        std::string expected_log_metrics("2018-08-02 12:08:06 164d89c1a56957b752540093e178 0 0 0 0 0 0 2 0\n");
        m.log(epoch+7s);
        RED_CHECK_FILE_CONTENTS(logmetrics1, expected_log_metrics);
    }
    {
        std::string expected_log_metrics(
            "2018-08-02 12:08:06 164d89c1a56957b752540093e178 0 0 0 0 0 0 2 0\n"
            "2018-08-02 12:08:11 164d89c1a56957b752540093e178 0 0 0 0 0 0 2 0\n");
        m.log(epoch+10s);
        RED_CHECK_FILE_CONTENTS(logmetrics1, expected_log_metrics);
    }

    RED_CHECK_WORKSPACE(wd);
}

RED_AUTO_TEST_CASE(TestVNCMetricsLogCycle2)
{
    WorkingDirectory wd("metrics_log_cycle2");

    {
        auto epoch = to_timeval(1533211681s);

        Metrics m( wd.dirname()
                , "164d89c1a56957b752540093e178"
                , "51614130003BD5522C94E637866E4D749DDA13706AC2610C6F77BBFE111F3A58"_av
                , "1C57BA616EEDA5C9D8FF2E0202BB087D0B5D865AC830F336CDB9804331095B31"_av
                , "EAF28B142E03FFC03A35676722BB99DBC21908F3CEA96A8DA6E3C2321056AC48"_av
                , "B079C9845904075BAC3DBE0A26CB7364CE0CC0A5F47DC082F44D221EBC6722B7"_av
                , epoch
                , 24h
                , 3s
                );
        VNCMetrics metrics(m);

        auto const logmetrics1 = wd.add_file("vnc_metrics-v1.0-2018-08-02.logmetrics");
        auto const logindex1 = wd.add_file("vnc_metrics-v1.0-2018-08-02.logindex");
        RED_CHECK_WORKSPACE(wd);

        RED_CHECK_FILE_CONTENTS(logmetrics1, ""_av);

        {
            metrics.right_click();
            m.log(epoch+0s);
            RED_CHECK_FILE_CONTENTS(logmetrics1, ""_av);
        }
        {
            m.log(epoch+1s);
            RED_CHECK_FILE_CONTENTS(logmetrics1, ""_av);
        }
        {
            m.log(epoch+2s);
            RED_CHECK_FILE_CONTENTS(logmetrics1, ""_av);
        }
        {
            std::string expected_log_metrics("2018-08-02 12:08:04 164d89c1a56957b752540093e178 0 0 0 0 0 0 1 0\n");
            m.log(epoch+3s);
            RED_CHECK_FILE_CONTENTS(logmetrics1, expected_log_metrics);
        }
    }

    RED_CHECK_WORKSPACE(wd);
}

RED_AUTO_TEST_CASE(TestVNCMetricsLogBasicIncrement)
{
    WorkingDirectory wd("metrics_log_basic_inc");

    auto epoch = to_timeval(1533211681s);

    Metrics m( wd.dirname()
          , "164d89c1a56957b752540093e178"
          , "51614130003BD5522C94E637866E4D749DDA13706AC2610C6F77BBFE111F3A58"_av
          , "1C57BA616EEDA5C9D8FF2E0202BB087D0B5D865AC830F336CDB9804331095B31"_av
          , "EAF28B142E03FFC03A35676722BB99DBC21908F3CEA96A8DA6E3C2321056AC48"_av
          , "B079C9845904075BAC3DBE0A26CB7364CE0CC0A5F47DC082F44D221EBC6722B7"_av
          , epoch
          , 24h
          , 5s
          );
    VNCMetrics metrics(m);

    auto const logmetrics1 = wd.add_file("vnc_metrics-v1.0-2018-08-02.logmetrics");
    auto const logindex1 = wd.add_file("vnc_metrics-v1.0-2018-08-02.logindex");
    RED_CHECK_WORKSPACE(wd);

    std::string expected_log_metrics("2018-08-02 12:08:06 164d89c1a56957b752540093e178 0 0 0 0 0 0 1 0\n");

    {
        epoch += 5s;
        metrics.right_click();
        m.log(epoch);
        RED_CHECK_FILE_CONTENTS(logmetrics1, expected_log_metrics);
    }
    {
        epoch += 5s;
        std::string expected_log_metrics_next("2018-08-02 12:08:11 164d89c1a56957b752540093e178 0 0 0 0 0 0 2 0\n");
        expected_log_metrics += expected_log_metrics_next;
        metrics.right_click();
        m.log(epoch);
        RED_CHECK_FILE_CONTENTS(logmetrics1, expected_log_metrics);
    }
    {
        epoch += 5s;
        std::string expected_log_metrics_next("2018-08-02 12:08:16 164d89c1a56957b752540093e178 0 0 0 0 0 0 2 1\n");
        expected_log_metrics += expected_log_metrics_next;
        metrics.left_click();
        m.log(epoch);
        RED_CHECK_FILE_CONTENTS(logmetrics1, expected_log_metrics);
    }
    {
        epoch += 5s;
        std::string expected_log_metrics_next("2018-08-02 12:08:21 164d89c1a56957b752540093e178 0 0 0 0 0 1 2 1\n");
        expected_log_metrics += expected_log_metrics_next;
        metrics.key_pressed();
        m.log(epoch);
        RED_CHECK_FILE_CONTENTS(logmetrics1, expected_log_metrics);
    }
    {
        epoch += 5s;
        std::string expected_log_metrics_next("2018-08-02 12:08:26 164d89c1a56957b752540093e178 0 0 0 0 4 1 2 1\n");
        expected_log_metrics += expected_log_metrics_next;
        metrics.mouse_move(0, 0);
        metrics.mouse_move(2, 2);
        m.log(epoch);
        RED_CHECK_FILE_CONTENTS(logmetrics1, expected_log_metrics);
    }

    RED_CHECK_WORKSPACE(wd);
}
