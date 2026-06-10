# Diagnostic Daemon

## Overview

The diagnostic daemon is the core user-space application for the industrial device simulator.

Its purpose is to simulate machine diagnostic data, manage machine states, detect faults, and write diagnostic events to a log file.

The daemon is designed as the foundation for later Buildroot image integration, persistent deployment on the Raspberry Pi target, and optional MQTT-based status publishing.

## Implemented files

```text
app/
├── Makefile
├── include/
│   └── diagnostic_daemon.h
└── src/
    └── diagnostic_daemon.c
```

## Current functionality

The current daemon implementation provides:

* a machine status data structure
* simulated temperature and vibration values
* a cycle counter
* machine state handling
* warning and fault detection
* fault code definitions
* latched fault behavior
* periodic status output
* event logging to `/tmp/diagnostic_events.log`
* clean shutdown using `SIGINT` or `SIGTERM`

## Machine states

The daemon currently supports the following machine states:

```text
MACHINE_STATE_STOPPED
MACHINE_STATE_STARTING
MACHINE_STATE_RUNNING
MACHINE_STATE_WARNING
MACHINE_STATE_FAULT
```

The daemon starts in `STARTING`, moves to `RUNNING`, and can transition to `WARNING` or `FAULT` depending on simulated diagnostic values.

When a fault occurs, the daemon remains latched in `FAULT` state until it is stopped or restarted.

## Fault codes

The daemon currently supports the following fault codes:

```text
FAULT_NONE
FAULT_OVER_TEMPERATURE
FAULT_HIGH_VIBRATION
FAULT_SENSOR_FAILURE
```

A simulated `SENSOR_FAILURE` fault is generated at cycle 12 to validate fault handling behavior.

## Event logging

The daemon writes diagnostic events to:

```text
/tmp/diagnostic_events.log
```

The log currently includes events such as:

```text
DAEMON_START
MACHINE_WARNING
MACHINE_FAULT
DAEMON_STOP
```

Example log output:

```text
Diagnostic daemon event log started
1970-01-01 00:01:05 event=DAEMON_START state=STARTING temperature=35.00C vibration=0.10g fault=NONE fault_code=0 cycle=0
1970-01-01 00:01:27 event=MACHINE_FAULT state=FAULT temperature=39.20C vibration=0.34g fault=SENSOR_FAILURE fault_code=1003 cycle=12
1970-01-01 00:01:29 event=MACHINE_FAULT state=FAULT temperature=39.20C vibration=0.34g fault=SENSOR_FAILURE fault_code=1003 cycle=13
1970-01-01 00:01:34 event=DAEMON_STOP state=FAULT temperature=39.20C vibration=0.34g fault=SENSOR_FAILURE fault_code=1003 cycle=15
```

On the Buildroot target, timestamps may start from `1970-01-01` if system time has not been synchronized. This does not affect diagnostic state handling or event logging validation.

## Build locally on the development VM

From the repository root:

```bash
make -C app
```

Run locally:

```bash
./app/diagnostic-daemon
```

Stop with:

```text
Ctrl+C
```

Example output:

```text
[diagnostic-daemon] starting
[diagnostic-daemon] version=0.2.0 state=STARTING temperature=35.35C vibration=0.12g fault=NONE fault_code=0 cycle=1
[diagnostic-daemon] version=0.2.0 state=RUNNING temperature=35.70C vibration=0.14g fault=NONE fault_code=0 cycle=2
[diagnostic-daemon] version=0.2.0 state=FAULT temperature=39.20C vibration=0.34g fault=SENSOR_FAILURE fault_code=1003 cycle=12
[diagnostic-daemon] stopping
```

## Cross-compile for Raspberry Pi target

From the repository root:

```bash
make -C app clean
make -C app CC=~/final-project-val-exx/buildroot/output/host/bin/arm-linux-gcc
```

Verify that the generated binary is for ARM:

```bash
file app/diagnostic-daemon
```

Expected result should indicate an ARM executable, for example:

```text
ELF 32-bit LSB executable, ARM
```

## Copy to Raspberry Pi target

The Raspberry Pi is accessed through USB/RNDIS using:

```text
Raspberry Pi IP: 192.168.7.2
```

Copy the daemon binary:

```bash
scp app/diagnostic-daemon root@192.168.7.2:/tmp/
```

## Run on target

Connect through SSH:

```bash
ssh root@192.168.7.2
```

Run the daemon:

```sh
chmod +x /tmp/diagnostic-daemon
/tmp/diagnostic-daemon
```

Stop with:

```text
Ctrl+C
```

Read the event log:

```sh
cat /tmp/diagnostic_events.log
```

## Target validation result

The daemon was successfully tested on the Raspberry Pi Zero 2 W running the custom Buildroot image.

Verified:

* USB/RNDIS SSH access works
* binary can be copied to the target using `scp`
* daemon runs directly on the Buildroot target
* periodic diagnostic output is printed
* event log is created at `/tmp/diagnostic_events.log`
* `DAEMON_START` event is logged
* simulated `SENSOR_FAILURE` fault is generated at cycle 12
* daemon enters `FAULT` state
* daemon remains latched in `FAULT` state after detection
* `DAEMON_STOP` event is logged on clean shutdown

## Next steps

Future work will extend this daemon with:

* Buildroot package integration
* automatic daemon startup from init scripts
* persistent log file location
* improved timestamp handling through system time synchronization
* optional MQTT publishing

