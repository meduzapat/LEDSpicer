# Adalight

Serial protocol for addressable LED strips (WS2812, WS2812B and compatible). Not a single product: any controller running Adalight-compatible firmware. `Adalight` derives from `DeviceSerial`, not `DeviceUSB`.

## Protocol

Every frame is a 6 byte header followed by raw RGB data.

| Byte | Content |
|---|---|
| 0-2 | magic `A` `d` `a` |
| 3 | LED count high byte |
| 4 | LED count low byte |
| 5 | checksum, `high ^ low ^ 0x55` |
| 6+ | 3 bytes per LED, in strip order |

The count field is the number of LEDs **minus one**. `transfer()` computes it as `(channels / 3) - 1`. A 30 LED strip sends `0x00 0x1D`.

LEDSpicer appends a trailing `\0` after the data and flushes the input queue with `tcflush(fd, TCIFLUSH)` after each frame.

Reference: [visualizing Adalight header information](https://www.partsnotincluded.com/visualizing-adalight-header-information/).

## Serial parameters

| Property | Value |
|---|---|
| Baud | 115200 |
| Framing | 8N1 |
| Reads | non-blocking |

## Port detection

Serial devices have no vendor-assigned identity the way HID boards do, so detection matches strings in sysfs. `Serial::findPortByUsbId()` scans `ttyUSB0`-`ttyUSB4` and `ttyACM0`-`ttyACM4`, reading `/sys/class/tty/<port>/device/uevent` and returning the first port whose uevent contains the search string.

Candidates in `ADALIGHT_PRODUCT_IDS`:

| Entry | Note |
|---|---|
| `2e8a/1075` | Raspberry Pi vendor, RP2040 class board |
| `2e8a/108f` | Raspberry Pi vendor |
| `2e8a/10da` | Raspberry Pi vendor |
| `838/8918` | |
| `ch341-uart` | matches the CH340/CH341 kernel driver name, not a USB ID |

This is a heuristic, not identification. These are generic USB-serial bridge chips shared with unrelated hardware, and the CH341 entry matches a driver rather than a device. A CH340-based board that is not an LED controller will be picked up. Set `port` explicitly in the config when more than one serial device is attached.

## Constraints

- RGB only. The protocol has no notion of a single channel, so `Adalight` cannot address individual LEDs and `LEDs.size()` must be a multiple of 3.
- No acknowledgement. The firmware sends nothing back, so a wrong LED count or a dropped frame is invisible to LEDSpicer. `transferFromConnection()` exists for debugging and is commented out in `transfer()`.
- Strip order is physical. Colour channel order (RGB vs GRB) is the firmware's business; set it there, not here.

## Quickstart

Hardware used by the author:

- Controllers: [Wemos D1 mini](https://www.wemos.cc/en/latest/d1/d1_mini.html), [Acustomarcade PICOCRT](https://acustomarcade.com/product/ws2812b-rgb-adapter-30-rgb)
- LEDs: WS2812 string, WS2812B 5V strip

For the D1 mini or similar, flash [WLED](https://github.com/Aircoookie/WLED) first, which leaves a wifi and USB controlled strip. Create a preset with the LEDs on but black and set it as the boot preset. Once the settings are right (mainly RGB order and maximum brightness) and the strip is tested, disable wifi. Changing anything after that requires reflashing WLED.
