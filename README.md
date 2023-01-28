# metricq-source-cmd

A simple MetricQ source that executes a set of commands that return floating point numbers and sends them as metrics.

This source is more a proof of concept than a production-ready thing for the following considerations.

# Security considerations

This source allows full code execution for anyone who:

- can manipulate the source configuration, or
- knows the source token and has permission to send to it RabbitMQ in the direct exchange

You should run it with limited privileges only!

# Performance considerations

The commands are executed synchronously in a single thread.
Using slow or many commands may block the whole source and cause connection issues.

# Configuration

Here's an example configuration.

```json
{
  "_id": "source-cmd",
  "metrics": {
    "localhost.random": {
      "command": "/bin/bash -c 'echo $RANDOM'",
      "unit": "",
      "interval": "500ms"
    },
    "localhost.date": {
      "command": "date +%s",
      "unit": "s",
      "interval": "1s"
    },
    "localhost.uptime": {
      "command": "bash -c 'uptime | grep -Eo [0-9.]+$'",
      "unit": "",
      "interval": "2s"
    }
  }
}
```

The source supports dynamic reconfiguration.
When reconfiguring, all timers are restarted, including existing unchanged metrics.