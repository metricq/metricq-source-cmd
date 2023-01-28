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
#pragma once

#include <metricq/source.hpp>
#include <metricq/timer.hpp>

class MetricExecutor
{
public:
    MetricExecutor(metricq::Metric<metricq::Source>& metric, metricq::Duration interval,
                   const std::string& command, const std::string& unit,
                   asio::io_service& io_service);
    ~MetricExecutor();

    void start();
    void cancel();

    metricq::Timer::TimerResult timeout_cb(std::error_code);

private:
    double execute_command();

    metricq::Duration interval;

    metricq::Timer timer_;
    std::string command_;
    std::string unit_;
    metricq::Metric<metricq::Source>& metric_;
};