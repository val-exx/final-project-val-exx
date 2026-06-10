# Buildroot Image Integration

## Overview

This document summarizes the integration of the `diagnostic-daemon` application into the custom Buildroot image for the Raspberry Pi Zero 2 W target.

Before this step, the daemon was built manually and copied to the target using `scp`.

After this integration, Buildroot builds the daemon as part of the image generation process and installs it directly into the target root filesystem.

## Goal

The goal of this issue was to make the diagnostic daemon available inside the generated Buildroot image without requiring manual copy steps.

The expected target location is:

```text
/usr/bin/diagnostic-daemon
```

## Buildroot external tree

A Buildroot external tree was added to the project repository.

Implemented files:

```text
Config.in
external.desc
external.mk
package/diagnostic-daemon/Config.in
package/diagnostic-daemon/diagnostic-daemon.mk
```

The external tree is identified by Buildroot as:

```text
diagnostic_daemon
```

The generated Buildroot external variable is:

```text
BR2_EXTERNAL_diagnostic_daemon_PATH
```

## Package structure

The custom Buildroot package is located under:

```text
package/diagnostic-daemon/
```

Package files:

```text
package/diagnostic-daemon/
├── Config.in
└── diagnostic-daemon.mk
```

## Package configuration

The package is enabled in Buildroot with:

```text
BR2_PACKAGE_DIAGNOSTIC_DAEMON=y
```

This was verified in the Buildroot `.config` file:

```bash
grep -n "BR2_PACKAGE_DIAGNOSTIC_DAEMON" .config
```

Expected result:

```text
BR2_PACKAGE_DIAGNOSTIC_DAEMON=y
```

## Buildroot package behavior

The package uses the project application source directory:

```text
app/
```

Buildroot copies the source into its build directory:

```text
output/build/diagnostic-daemon-0.2.0/
```

Then it builds the daemon using the Buildroot target cross-compiler:

```text
output/host/bin/arm-linux-gcc
```

The final binary is installed into the target root filesystem at:

```text
output/target/usr/bin/diagnostic-daemon
```

## Build command

From the Buildroot directory:

```bash
cd ~/final-project-val-exx/buildroot
make BR2_EXTERNAL=~/final-project-val-exx diagnostic-daemon
```

The package build was successful and showed:

```text
>>> diagnostic-daemon 0.2.0 Building
>>> diagnostic-daemon 0.2.0 Installing to target
```

## Target filesystem verification

The daemon binary was verified inside the generated target filesystem:

```bash
ls -l output/target/usr/bin/diagnostic-daemon
file output/target/usr/bin/diagnostic-daemon
```

Validated result:

```text
output/target/usr/bin/diagnostic-daemon: ELF 32-bit LSB shared object, ARM, EABI5
```

## Image generation

After the package was built and installed into the target filesystem, the full Buildroot image was regenerated:

```bash
make BR2_EXTERNAL=~/final-project-val-exx
```

The generated image is:

```text
buildroot/output/images/sdcard.img
```

The image generation completed successfully and produced the final SD card image.

## Target validation

The generated `sdcard.img` was flashed to the Raspberry Pi Zero 2 W.

After booting the target, SSH access over USB/RNDIS was verified:

```bash
ssh root@192.168.7.2
```

On the target, the daemon was verified to be available directly from the root filesystem:

```sh
which diagnostic-daemon
ls -l /usr/bin/diagnostic-daemon
```

Validated result:

```text
/usr/bin/diagnostic-daemon
-rwxr-xr-x 1 root root 9520 Jun 10 2026 /usr/bin/diagnostic-daemon
```

This confirms that the daemon is included in the Buildroot image and no manual `scp` copy is required.

## Runtime validation

The daemon was executed directly on the target:

```sh
diagnostic-daemon
```

The daemon successfully:

* started on the Buildroot target
* printed simulated diagnostic status
* generated a simulated `SENSOR_FAILURE` fault at cycle 12
* remained latched in `FAULT` state
* wrote events to `/tmp/diagnostic_events.log`
* exited cleanly with `Ctrl+C`

Example event log:

```text
Diagnostic daemon event log started
1970-01-01 00:02:35 event=DAEMON_START state=STARTING temperature=35.00C vibration=0.10g fault=NONE fault_code=0 cycle=0
1970-01-01 00:02:57 event=MACHINE_FAULT state=FAULT temperature=39.20C vibration=0.34g fault=SENSOR_FAILURE fault_code=1003 cycle=12
1970-01-01 00:02:59 event=MACHINE_FAULT state=FAULT temperature=39.20C vibration=0.34g fault=SENSOR_FAILURE fault_code=1003 cycle=13
1970-01-01 00:03:00 event=DAEMON_STOP state=FAULT temperature=39.20C vibration=0.34g fault=SENSOR_FAILURE fault_code=1003 cycle=13
```

## Notes

The target log timestamps start from `1970-01-01` because system time synchronization is not configured yet.

This does not affect the validation of:

* Buildroot package integration
* daemon installation into the image
* runtime execution
* fault handling
* event logging

## Result

The diagnostic daemon is now fully integrated into the Buildroot image.

The Raspberry Pi target can run:

```sh
diagnostic-daemon
```

directly after boot, without manually copying the binary.
