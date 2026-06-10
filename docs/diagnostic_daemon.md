# Diagnostic Daemon Core

## Overview

The diagnostic daemon is the core user-space application for the industrial device simulator.

Its current purpose is to provide a minimal diagnostic loop that simulates machine status data and prints periodic diagnostic output.

This initial implementation is part of Sprint 2 and prepares the project for later integration with simulated machine status, fault handling, event logging, and Buildroot image integration.

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

* a basic machine status data structure
* simulated temperature and vibration values
* a cycle counter
* a simple fault transition when temperature reaches a threshold
* periodic status output
* clean shutdown using `SIGINT` or `SIGTERM`

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
[diagnostic-daemon] version=0.1.0 state=RUNNING temperature=35.20C vibration=0.11g fault_code=0 cycle=1
[diagnostic-daemon] version=0.1.0 state=RUNNING temperature=35.40C vibration=0.12g fault_code=0 cycle=2
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

## Target validation result

The daemon was successfully tested on the Raspberry Pi Zero 2 W running the custom Buildroot image.

Verified:

* USB/RNDIS SSH access works
* binary can be copied to the target using `scp`
* daemon runs directly on the Buildroot target
* periodic diagnostic output is printed
* daemon exits cleanly with `Ctrl+C`

## Next steps

Future work will extend this daemon with:

* simulated machine operating states
* fault handling
* event logging
* persistent log files
* Buildroot package integration
* optional MQTT publishing

