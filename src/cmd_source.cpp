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

#include <metricq/types.hpp>

#include <fmt/format.h>

using Log = metricq::logger::nitro::Log;

CmdSource::CmdSource(const std::string& manager_host, const std::string& token)
: metricq::Source(token), signals_(io_service, SIGINT, SIGTERM)
{
    Log::debug() << "CmdSource::CmdSource() called";

    // Register signal handlers so that the daemon may be shut down.
    signals_.async_wait(
        [this](auto, auto signal)
        {
            if (!signal)
            {
                return;
            }
            Log::info() << "Caught signal " << signal << ". Shutdown.";
            cancel_executors();

            Log::info() << "closing source";
            stop();
        });

    connect(manager_host);
}

CmdSource::~CmdSource()
{
}

void CmdSource::on_source_config(const metricq::json& config)
{
    Log::debug() << "CmdSource::on_source_config() called";
    Log::trace() << "config: " << config;
    auto& executor_metrics = config.at("metrics");
    for (auto it = executor_metrics.begin(); it != executor_metrics.end(); ++it)
    {
        const auto& executor_config = it.value();
        const std::string& metric_name = it.key();

        if (executor_config.is_object())
        {
            auto& metric = (*this)[metric_name];
            std::string unit = executor_config.at("unit");
            std::string command = executor_config.at("command");
            metricq::Duration interval =
                metricq::duration_parse(executor_config.at("interval").get<std::string>());

            metric_executors_.try_emplace(metric_name, metric, interval, command, unit, io_service);
        }
        else
        {
            throw std::runtime_error("invalid configuration for metric executor");
        }
    }
}

void CmdSource::on_source_ready()
{
    Log::debug() << "CmdSource::on_source_ready() called";
    for (auto& metric_executor : metric_executors_)
    {
        metric_executor.second.start();
    }

    running_ = true;
}

void CmdSource::on_error(const std::string& message)
{
    Log::debug() << "CmdSource::on_error() called";
    Log::error() << "Shit hits the fan: " << message;
    signals_.cancel();
    cancel_executors();
}

void CmdSource::on_closed()
{
    Log::debug() << "CmdSource::on_closed() called";
    signals_.cancel();
    cancel_executors();
}

void CmdSource::cancel_executors()
{
    for (auto& metric_executor : metric_executors_)
    {
        metric_executor.second.cancel();
    }
}