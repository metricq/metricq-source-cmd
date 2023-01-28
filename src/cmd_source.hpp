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

#include "metric_executor.hpp"

#include <metricq/source.hpp>
#include <metricq/timer.hpp>

#include <asio/signal_set.hpp>

#include <atomic>
#include <system_error>

class CmdSource : public metricq::Source
{
public:
    CmdSource(const std::string& manager_host, const std::string& token);
    ~CmdSource();

    void on_error(const std::string& message) override;
    void on_closed() override;

private:
    void on_source_config(const metricq::json& config) override;
    void on_source_ready() override;

    void start_executors();

    asio::signal_set signals_;

    bool running_ = false;

    std::unordered_map<std::string, MetricExecutor> metric_executors_;
};
