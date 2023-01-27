// metricq-source-cmd
// Copyright (C) 2023 ZIH, Technische Universitaet Dresden, Federal Republic of Germany
//
// All rights reserved.
//
// This file is part of metricq-source-cmd.
//
// metricq-source-cmd is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// metricq-source-cmd is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with metricq-source-cmd.  If not, see <http://www.gnu.org/licenses/>.
#include "cmd_source.hpp"

#include <metricq/logger/nitro.hpp>

#include <nitro/options/parser.hpp>

#include <iostream>
#include <string>

using Log = metricq::logger::nitro::Log;

int main(int argc, char* argv[])
{
    metricq::logger::nitro::set_severity(nitro::log::severity_level::info);

    nitro::options::parser parser;
    parser.option("server", "The metricq management server to connect to.")
        .default_value("amqp://localhost")
        .short_name("s");
    parser.option("token", "The token used for source authentication against the metricq manager.")
        .default_value("source-cmd");
    parser.toggle("verbose").short_name("v");
    parser.toggle("trace").short_name("t");
    parser.toggle("quiet").short_name("q");
    parser.toggle("help").short_name("h");

    try
    {
        auto options = parser.parse(argc, argv);

        if (options.given("help"))
        {
            parser.usage();
            return 0;
        }

        if (options.given("trace"))
        {
            metricq::logger::nitro::set_severity(nitro::log::severity_level::trace);
        }
        else if (options.given("verbose"))
        {
            metricq::logger::nitro::set_severity(nitro::log::severity_level::debug);
        }
        else if (options.given("quiet"))
        {
            metricq::logger::nitro::set_severity(nitro::log::severity_level::warn);
        }

        metricq::logger::nitro::initialize();

        CmdSource source(options.get("server"), options.get("token"));
        Log::info() << "starting main loop.";
        source.main_loop();
        Log::info() << "exiting main loop.";
    }
    catch (nitro::options::parsing_error& e)
    {
        std::cerr << e.what() << '\n';
        parser.usage();
        return 1;
    }
    catch (std::exception& e)
    {
        Log::error() << "Unhandled exception: " << e.what();
        return 2;
    }
}
