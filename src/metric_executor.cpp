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
#include "metric_executor.hpp"

#include <metricq/logger/nitro.hpp>

#include <metricq/types.hpp>

#include <cstdio>

using Log = metricq::logger::nitro::Log;

MetricExecutor::MetricExecutor(metricq::Metric<metricq::Source>& metric, metricq::Duration interval, const std::string& command, const std::string& unit, asio::io_service& io_service):
  interval(interval), timer_(io_service), command_(command), unit_(unit), metric_(metric)
{

}

MetricExecutor::~MetricExecutor() {}

metricq::Timer::TimerResult MetricExecutor::timeout_cb(std::error_code)
{
    Log::debug() << "sending metrics...";
    metric_.chunk_size(1);
    double value = execute_command();
    auto current_time = metricq::Clock::now();
    metric_.send({ current_time, value });
    return metricq::Timer::TimerResult::repeat;
}

void MetricExecutor::start() {
    // define metadata
    metric_.metadata.unit(unit_);
    metric_.metadata.rate(1 / (interval.count() * 1e-9));
    metric_.metadata["description"] = command_;

    // start timer
    timer_.start([this](auto err) { return this->timeout_cb(err); }, interval);
}

void MetricExecutor::cancel() {
    timer_.cancel();
}

double MetricExecutor::execute_command()
{
    // FROM https://stackoverflow.com/a/478960
    std::array<char, 128> buffer{};
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command_.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    return std::strtod(result.c_str(), nullptr);
}