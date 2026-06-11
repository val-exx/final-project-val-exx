# Industrial Diagnostic Device Simulator and Logger on Raspberry Pi Zero using Buildroot

This repository contains my individual final project for the **Advanced Embedded Linux Development** course.

The project implements a custom embedded Linux system for a **Raspberry Pi Zero 2 W** using **Buildroot**.
The system includes a C-based diagnostic daemon that simulates industrial machine status, handles faults, and writes diagnostic events to a log file.

## Project overview

The final system provides:

* custom Buildroot image for Raspberry Pi Zero 2 W
* USB/RNDIS network access over `usb0`
* SSH access to the target at `192.168.7.2`
* custom Buildroot external tree
* custom Buildroot package for `diagnostic-daemon`
* diagnostic daemon installed in `/usr/bin/diagnostic-daemon`
* simulated machine status
* fault handling with latched fault behavior
* event logging to `/tmp/diagnostic_events.log`
* final validation on real Raspberry Pi hardware

## Repository structure

```text
app/
├── Makefile
├── include/
│   └── diagnostic_daemon.h
└── src/
    └── diagnostic_daemon.c

configs/
└── val_rpi_zero2w_defconfig

docs/
├── buildroot_integration.md
├── buildroot_setup.md
├── diagnostic_daemon.md
├── protocol.md.md
└── final_system_test.md

package/
└── diagnostic-daemon/
    ├── Config.in
    └── diagnostic-daemon.mk

rootfs_overlay/
└── etc/init.d/
    ├── K41wifi.disabled
    └── S40usb-gadget

Config.in
external.desc
external.mk
```

## Diagnostic daemon

The diagnostic daemon is a user-space C application that simulates an industrial machine diagnostic system.

Current functionality includes:

* machine state simulation
* temperature and vibration simulation
* warning and fault detection
* simulated `SENSOR_FAILURE` fault at cycle 12
* latched `FAULT` state behavior
* event logging
* clean shutdown using `SIGINT` or `SIGTERM`

The daemon is installed in the final Buildroot image at:

```text
/usr/bin/diagnostic-daemon
```

It can be executed on the target with:

```sh
diagnostic-daemon
```

## Buildroot integration

The project uses a Buildroot external tree.

The custom package is located in:

```text
package/diagnostic-daemon/
```

The package is enabled through:

```text
BR2_PACKAGE_DIAGNOSTIC_DAEMON=y
```

The final Buildroot configuration is saved in:

```text
configs/val_rpi_zero2w_defconfig
```

The full Buildroot tree is not committed to this repository.
Instead, the repository contains the project-specific source code, Buildroot external tree, package files, overlays, documentation, and final defconfig.

## Final target validation

The final image was tested on a Raspberry Pi Zero 2 W.

Validated target system:

```text
Linux buildroot 6.12.61-v7 #1 SMP Fri Jun 5 18:00:48 CEST 2026 armv7l GNU/Linux
```

Validated USB/RNDIS interface:

```text
usb0 inet 192.168.7.2/24
```

Validated daemon installation:

```text
/usr/bin/diagnostic-daemon
```

Validated runtime behavior:

```text
[diagnostic-daemon] version=0.2.0 state=FAULT temperature=39.20C vibration=0.34g fault=SENSOR_FAILURE fault_code=1003 cycle=12
```

Validated event log:

```text
event=DAEMON_START
event=MACHINE_FAULT
event=DAEMON_STOP
```

## Documentation

Detailed project documentation is available in:

* `docs/buildroot_setup.md` - initial Buildroot and Raspberry Pi Zero 2 W setup
* `docs/diagnostic_daemon.md` - diagnostic daemon implementation and validation
* `docs/protocol.md` - diagnostic protocol draft documentation
* `docs/buildroot_integration.md` - Buildroot external tree and package integration
* `docs/final_system_test.md` - final target validation and test results

The project wiki also contains:

* [Project Overview](https://github.com/val-exx/final-project-val-exx/wiki/Project-Overview)
* [Final Project Schedule](https://github.com/val-exx/final-project-val-exx/wiki/Final-Project-Schedule)

## Final result

The final system demonstrates a custom Buildroot image running on Raspberry Pi Zero 2 W with an integrated diagnostic daemon that simulates machine status, handles faults, and writes diagnostic events to a log file.
