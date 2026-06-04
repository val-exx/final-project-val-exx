# Buildroot Setup for Raspberry Pi Zero

## Target Hardware

The target hardware platform is Raspberry Pi Zero.

## Build System

The project uses Buildroot to generate a custom embedded Linux image.

## Initial Buildroot Configuration

The initial Buildroot configuration is based on the Raspberry Pi Zero defconfig:

```bash
make raspberrypi0_defconfig
```

## Build Command

After selecting the target configuration, the image can be build using:
```bash
make 
```

## Expected Output

The generated images are expcted under: 
```
output/images/
```
Typical output files include:
```
sdcard.img
zImage
rootfs.tar
```

## Flashing the Image

The generated ```sdcard.img``` can be written to a microSD card using a tool such ad ```dd``` or Raspberry Pi Imager.

Examples using ```dd```:
```bash
sudo dd if=output/images/sdcard.img of=/dev/sdX bs=4M status=progress conv=fsync
```

Where ```/dev/sdX/``` identifies the microSD device. 

## Target Access

The access method will be confirmed during testing. Possible options are:
* serial console
* USB networking / USB gadget mode
* USB Ethernet adapter
* Wi-Fi, if using Raspberry Pi Zero W

## First Build Result

The first baseline Buildroot image was generated using:

```bash
make raspberrypi0_defconfig
make
```

The build completed successfully and generated the Raspberry Pi Zero SD card image:
```
output/images/sdcard.img
```

The next step is to flash image to a microSD card and verify boot on the Raspberry Pi Zero
