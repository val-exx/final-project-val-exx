# Diagnostic Protocol Draft

## Overview

The diagnostic daemon exposes a simple text-based TCP protocol.

A client sends one command per line. The daemon processes the command and returns a text response.

The protocol simulates a basic industrial diagnostic interface used to read machine status, diagnostic values, fault codes, and event logs from an embedded diagnostic node.

## Supported Commands

| Command | Description | Example Response |
|---|---|---|
| `HELP` | Lists supported commands | `OK COMMANDS READ_STATUS READ_TEMP READ_CURRENT READ_VIBRATION READ_FAULTS INJECT_FAULT CLEAR_FAULTS READ_LOG` |
| `READ_STATUS` | Reads the current simulated machine status | `OK STATUS RUNNING` |
| `READ_TEMP` | Reads the simulated temperature value | `OK TEMP 42` |
| `READ_CURRENT` | Reads the simulated motor current value | `OK CURRENT 4.8` |
| `READ_VIBRATION` | Reads the simulated vibration level | `OK VIBRATION 12` |
| `READ_FAULTS` | Reads the active fault code | `OK FAULT NONE` |
| `INJECT_FAULT <FAULT_CODE>` | Injects a simulated fault | `OK FAULT_SET OVERHEAT` |
| `CLEAR_FAULTS` | Clears active simulated faults | `OK FAULT_CLEARED` |
| `READ_LOG` | Reads recent diagnostic log entries | `OK LOG ...` |

## Machine States

| State | Description |
|---|---|
| `IDLE` | Machine is powered but not operating |
| `RUNNING` | Machine is operating normally |
| `WARNING` | Machine is operating with abnormal but non-critical values |
| `FAULT` | Machine has an active fault condition |
| `MAINTENANCE` | Machine is in maintenance mode |

## Fault Codes

| Fault Code | Description |
|---|---|
| `NONE` | No active fault |
| `OVERHEAT` | Simulated temperature fault |
| `OVERCURRENT` | Simulated motor current fault |
| `HIGH_VIBRATION` | Simulated vibration fault |
| `COMMUNICATION_LOSS` | Simulated communication fault |
| `SENSOR_FAILURE` | Simulated sensor failure |

## Response Format

Successful responses start with:

```text
OK
```

Error responses start with:

```text
ERROR
```

Examples:

```text
OK STATUS RUNNING
OK TEMP 42
OK FAULT_SET OVERHEAT
ERROR UNKNOWN_COMMAND
ERROR INVALID_FAULT_CODE
```

## Normal Operation Scenario

1. Client connects to the Raspberry Pi Zero diagnostic daemon.
2. Client sends `READ_STATUS`.
3. Daemon responds with `OK STATUS RUNNING`.
4. Client sends `READ_TEMP`.
5. Daemon responds with a simulated temperature value.
6. Client sends `READ_FAULTS`.
7. Daemon responds with `OK FAULT NONE`.

## Fault Injection Scenario

1. Client sends `INJECT_FAULT OVERHEAT`.
2. Daemon updates the machine state to `FAULT`.
3. Daemon logs the fault event.
4. Daemon responds with `OK FAULT_SET OVERHEAT`.
5. Client sends `READ_STATUS`.
6. Daemon responds with `OK STATUS FAULT`.
7. Client sends `CLEAR_FAULTS`.
8. Daemon clears the active fault and returns to a non-fault state.

## Notes

This protocol is an initial draft and may be updated during implementation.
