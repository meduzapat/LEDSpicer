# WolfWare Tech

USB vendor ID `0x03EB`. This is Atmel's generic vendor ID, not exclusive to WolfWare Tech, so the product ID must always be checked as well. `USB::connect()` matches both.

| Product | PID | Boards | Implemented here |
|---|---|---|---|
| Howler | `6800`-`6803` | 4 | `Howler.{hpp,cpp}` |

Verified against [libhowler](https://github.com/Mokosha/libhowler) (`howler.h`, `usb_linux.c`).

## Howler

Arcade controller with 96 LED channels, 32 RGB LEDs.

| Group | RGB LEDs | Channels |
|---|---|---|
| Buttons | 26 | 78 |
| Joysticks | 4 | 12 |
| High power outputs | 2 | 6 |
| Total | 32 | 96 |

## Transport

Unlike every other USB board here, the Howler uses interrupt transfers, not control transfers. `Howler::send()` overrides `USB::send()` for this.

| Property | Value |
|---|---|
| Interface | 0 |
| OUT endpoint | `0x02` |
| IN endpoint | `0x81` |
| Transfer size | 24 bytes, fixed, both directions |

The header names these `HOWLER_IN_EP 0x02` and `HOWLER_OUT_EP 0x81`, which reads as the device's point of view: data goes *in to* the Howler on `0x02` and *out of* it on `0x81`. USB names endpoints from the host's point of view, where bit 7 of the address is the direction: `0x02` is OUT (host to device) and `0x81` is IN (device to host). So `send()` writing to `HOWLER_IN_EP` is correct, and looks wrong at a glance. `HOWLER_OUT_EP` is unused; nothing reads from the board.

## Message format

Every command is 24 bytes and starts with the magic byte `0xCE`.

| Byte | Content |
|---|---|
| 0 | `0xCE` |
| 1 | command |
| 2+ | command arguments, zero padded to 24 |

`HOWLER_WVALUE` is defined as `0x00CE` and used both as the nominal control value and, truncated to `uint8_t`, as this magic byte.

### Command set

| Code | Command | Used here |
|---|---|---|
| `0x01` | set RGB LED | no |
| `0x02` | set individual LED | no |
| `0x03` | set input | no |
| `0x04` | get input | no |
| `0x05` | set default | no |
| `0x06` | set global brightness | no |
| `0x07` | set RGB LED default | no |
| `0x08` | get RGB LED | no |
| `0x09` | set RGB LED bank | yes |
| `0xA0` | get firmware revision | no |
| `0xAC` | get accelerometer data | no |

The board carries an accelerometer readable with `0xAC`, and can store a power-on default LED state with `0x05` and `0x07`. Neither is exposed by LEDSpicer.

### Bank writes

The 96 channels are six banks of 16. One full frame is six `0x09` messages.

| Byte | Content |
|---|---|
| 0 | `0xCE` |
| 1 | `0x09` |
| 2 | bank number, 1 to 6 |
| 3-18 | 16 channel values |
| 19-23 | zero padding |

Banks are not contiguous slices of the channel array. `howlerBankA` and `howlerBankB` in `Howler.hpp` hold the scatter tables: odd banks (1, 3, 5) take one interleave, even banks (2, 4, 6) another, and the `row` argument (0, 1, 2) selects the red, green or blue plane. libhowler carries the same tables in `led_bank_tables.c` as `howler_button_to_bank`, `howler_joystick_to_bank` and `howler_hp_led_to_bank`.

Channel layout in `drawHardwareLedMap()`:

| Channels | Assignment |
|---|---|
| 0-11 | joysticks 1 to 4, 3 each |
| 12-89 | buttons 1 to 26, 3 each |
| 90-95 | high power outputs 1 and 2, 3 each |

## Known gaps

- Endpoint macros are named from the device's point of view, not the host's, so writes go to `HOWLER_IN_EP`. Correct, but it reads as a mistake.
- Global brightness (`0x06`) is not used, so LEDSpicer drives full range through the bank values only.
- No firmware revision check, so board revision differences would go unnoticed.
- libhowler lists PIDs `6800`-`6803`, matching the 4 board maximum here. Not verified on more than one board.
