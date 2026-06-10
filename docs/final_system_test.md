# Final System Test

## Overview

This document summarizes the final system validation for the industrial diagnostic device simulator running on a Raspberry Pi Zero 2 W with a custom Buildroot image.

The final test verifies that the complete system works end-to-end:

```text
Buildroot image -> Raspberry Pi boot -> USB/RNDIS SSH access -> diagnostic daemon execution -> fault handling -> event logging
```

## Test environment

Target hardware:

```text
Raspberry Pi Zero 2 W
```

Operating system:

```text
Custom Buildroot image
```

Kernel verified on target:

```text
Linux buildroot 6.12.61-v7 #1 SMP Fri Jun 5 18:00:48 CEST 2026 armv7l GNU/Linux
```

Network access method:

```text
USB/RNDIS Ethernet gadget
```

Target IP address:

```text
192.168.7.2/24
```

Host-side access:

```bash
ssh root@192.168.7.2
```

## System verification

The target system booted successfully using the final Buildroot image.

Command used:

```sh
uname -a
```

Validated result:

```text
Linux buildroot 6.12.61-v7 #1 SMP Fri Jun 5 18:00:48 CEST 2026 armv7l GNU/Linux
```

## USB/RNDIS network verification

The USB network interface was verified on the target.

Command used:

```sh
ip addr show usb0
```

Validated result:

```text
usb0: <BROADCAST,MULTICAST,UP,LOWER_UP>
inet 192.168.7.2/24 scope global usb0
```

This confirms that the Raspberry Pi can be accessed over USB/RNDIS using SSH.

## Diagnostic daemon installation verification

The diagnostic daemon was verified to be installed directly in the target root filesystem.

Commands used:

```sh
which diagnostic-daemon
ls -l /usr/bin/diagnostic-daemon
```

Validated result:

```text
/usr/bin/diagnostic-daemon
-rwxr-xr-x 1 root root 9520 Jun 10 2026 /usr/bin/diagnostic-daemon
```

This confirms that the daemon is included in the Buildroot image and does not need to be copied manually with `scp`.

## Runtime validation

The diagnostic daemon was executed directly on the target.

Command used:

```sh
diagnostic-daemon
```

Validated behavior:

```text
[diagnostic-daemon] starting
[diagnostic-daemon] version=0.2.0 state=STARTING temperature=35.35C vibration=0.12g fault=NONE fault_code=0 cycle=1
[diagnostic-daemon] version=0.2.0 state=RUNNING temperature=35.70C vibration=0.14g fault=NONE fault_code=0 cycle=2
[diagnostic-daemon] version=0.2.0 state=FAULT temperature=39.20C vibration=0.34g fault=SENSOR_FAILURE fault_code=1003 cycle=12
[diagnostic-daemon] version=0.2.0 state=FAULT temperature=39.20C vibration=0.34g fault=SENSOR_FAILURE fault_code=1003 cycle=13
[diagnostic-daemon] stopping
```

The daemon correctly:

* starts on the target
* prints periodic simulated diagnostic status
* progresses through machine cycles
* generates a simulated `SENSOR_FAILURE` fault at cycle 12
* enters `FAULT` state
* remains latched in `FAULT` state
* stops cleanly with `Ctrl+C`

## Event log validation

The event log was verified on the target.

Command used:

```sh
cat /tmp/diagnostic_events.log
```

Validated result:

```text
Diagnostic daemon event log started
1970-01-01 00:00:48 event=DAEMON_START state=STARTING temperature=35.00C vibration=0.10g fault=NONE fault_code=0 cycle=0
1970-01-01 00:01:10 event=MACHINE_FAULT state=FAULT temperature=39.20C vibration=0.34g fault=SENSOR_FAILURE fault_code=1003 cycle=12
1970-01-01 00:01:12 event=MACHINE_FAULT state=FAULT temperature=39.20C vibration=0.34g fault=SENSOR_FAILURE fault_code=1003 cycle=13
1970-01-01 00:01:14 event=MACHINE_FAULT state=FAULT temperature=39.20C vibration=0.34g fault=SENSOR_FAILURE fault_code=1003 cycle=14
1970-01-01 00:01:15 event=DAEMON_STOP state=FAULT temperature=39.20C vibration=0.34g fault=SENSOR_FAILURE fault_code=1003 cycle=14
```

The log confirms:

* `DAEMON_START` is logged
* `MACHINE_FAULT` is logged
* the simulated `SENSOR_FAILURE` fault is recorded
* the daemon remains latched in `FAULT`
* `DAEMON_STOP` is logged after clean shutdown

## Screenshot evidence

Screenshots were collected during final validation and attached to the final GitHub issue.

The screenshots show:

* system information and USB/RNDIS network configuration
* daemon installation in `/usr/bin/diagnostic-daemon`
* runtime fault handling and event log output

## Notes

The event log timestamps start from `1970-01-01` because system time synchronization is not configured yet.

This does not affect the validation of:

* Buildroot image boot
* USB/RNDIS access
* daemon installation
* daemon runtime behavior
* simulated fault handling
* event logging

## Final result

The final system test was completed successfully.

The project demonstrates a custom Buildroot image running on Raspberry Pi Zero 2 W with an integrated diagnostic daemon that simulates machine status, handles faults, and writes diagnostic events to a log file.
