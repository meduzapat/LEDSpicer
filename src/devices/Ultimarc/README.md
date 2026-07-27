# Ultimarc

USB vendor ID `0xD209` for every board below. Verified against [PacDriveSDK](https://github.com/benbaker76/PacDriveSDK) (`dll/PacDrive.h`, `dll/PacDrive.cpp`), [Ultimarc-linux](https://github.com/katie-snow/Ultimarc-linux), [QtPyUltimarc](https://github.com/katie-snow/QtPyUltimarc) and `lsusb` output in issues #16 and #17.

## Product codes

| Product | PID block | Boards | Implemented here |
|---|---|---|---|
| PacDrive | `1500` | 4, by `bcdDevice` | `PacDrive.{hpp,cpp}` |
| PacLed64 | `1401`-`1408` | see note | `PacLed64.{hpp,cpp}` |
| NanoLed | `1481`-`1484` | 4 | `NanoLed.{hpp,cpp}` |
| I-PAC Ultimate I/O | `0410`-`0413` | 2, block also encodes the mode | `Ultimate.{hpp,cpp}` |
| ServoStik | `1700` | by bus position | `restrictors/ServoStik.{hpp,cpp}` |
| UltraStik360 (2015+) | `0511` | 4 | `restrictors/UltraStik360.{hpp,cpp}` |
| UltraStik360 (pre 2015) | `0501` | 4 | not handled, interface is 0 not 2 |
| U-HID | `1501`-`1508` | 8 | not implemented |
| USB Button | `1200` | 1 | not implemented |
| Trackball / Spinner | `15a1` | 1 | not implemented, input only, no LEDs |

The SDK accepts `1401`-`1408` for PacLed64, but an SDK accept range is not a board count: the Ultimate block is 4 codes for 2 boards. `PAC_LED64_MAX_BOARDS` is 4 here and no second source was found. Settle it by setting board ID 5 in the Ultimarc tool and checking whether it enumerates as `d209:1405`.

## Transport

All boards are HID. LEDs are written with a control transfer, never an endpoint.

| Field | Value |
|---|---|
| `bmRequestType` | `0x21` (host to device, class, interface) |
| `bRequest` | `0x09` (SET_REPORT) |
| `wValue` | `0x0200 \| reportId` |
| `wIndex` | interface number |

The report ID appears twice, in the low byte of `wValue` and as the first payload byte. Ultimarc-linux and QtPyUltimarc both do this and the boards expect it.

## PacDrive

16 on/off outputs, no dimming. `Monochromatic` applies a `changePoint` threshold to turn LEDSpicer intensities into bits.

| Property | Value |
|---|---|
| Interface | 0 |
| `wValue` | `0x0200` (report ID 0) |
| Message | 4 bytes |
| Board ID | `bcdDevice`, not the PID (`isProductBasedId()` returns false) |

Message layout:

| Byte | SDK | LEDSpicer |
|---|---|---|
| 0 | `0x00` | `0x00` |
| 1 | `0xdd` | `0x00` |
| 2 | outputs 8-15 bitmask | outputs 8-15 bitmask |
| 3 | outputs 0-7 bitmask | outputs 0-7 bitmask |

Byte 1 differs. The SDK writes the same `0xdd` marker that ServoStik uses; LEDSpicer writes zero and works in the field, so the firmware appears to ignore it. Not verified on hardware.

PacDrive is the only Ultimarc board that identifies its position through `bcdDevice`. Ultimarc ships it configured for up to 4 boards, more on request.

Confirmed by `lsusb -v`: a board 1 unit reports `bcdDevice 0.01`, that is `0x0001`, so the `desc.bcdDevice == boardId` test in `USB::connect()` matches against a plain decimal `boardId`. The comparison is BCD against decimal and only stays correct while `boardId` is 9 or less, which the 4 board maximum guarantees. It also publishes a single HID interface numbered 0, matching `PAC_DRIVE_INTERFACE`, one IN endpoint at `0x81`, a 33 byte report descriptor, and no OUT endpoint.

`bcdDevice` therefore means two different things across this vendor: a board identifier on PacDrive, and a firmware version on the Ultimate I/O, where it carries no usable information. Nothing else keys off it.

## PacLed64 and NanoLed

Both use the pair stream, implemented in `FF00SharedCode` on top of `Ultimarc::transferPairs()`.

| Property | PacLed64 | NanoLed |
|---|---|---|
| LEDs | 64 | 60 |
| Interface | 1 | 1 |
| `wValue` | `0x0200` (report ID 0) | `0x0200` |
| Message | 2 bytes | 2 bytes |
| PID | `0x1401 + boardId - 1` | `0x1481 + boardId - 1` |

Message is `{command, value}`. Commands:

| Command | Meaning |
|---|---|
| `0x00`-`0x3F` | LED index, value is the intensity |
| `0x40` | flash speed (PacLed64 only) |
| `0x80` | all LEDs, value is the intensity |
| `0x89` | random states |
| `0xC0` | fade rate, value is the rate |
| `0xFE` | start of a full stream, followed by 32 (PacLed64) or 30 (NanoLed) pairs |

`group | 0x80` addresses a group of 8 outputs, with the group number shifted by 8 above group 8 (`(group > 8 ? group + 8 : group) | 0x80`). Not used here.

Both constructors pass interface 0 and `FF00SharedCode::afterConnect()` is empty, so the interface stays 0 rather than the 1 the headers define. **Interface 0 is correct and the constant is wrong.** A PacLed64 `lsusb -v` shows `bNumInterfaces 1`, a single HID interface numbered 0, one IN endpoint at `0x81`, and a report descriptor of 35 bytes. There is no interface 1 to claim. `PAC_LED64_INTERFACE` and `NANO_LED_INTERFACE` are dead constants.

The SDK rejects a NanoLed that does not present the old driver collection, so NanoLed has no fast path.

## I-PAC Ultimate I/O

Keyboard and joystick encoder plus 96 LED channels (32 RGB), 256 levels. The only Ultimarc board whose USB identity changes at runtime.

### Multi-Mode

Firmware 1.50+ switches operating mode on the board itself. Mode is not a config value and cannot be forced from the host.

| Mode | Presents as | PID | LEDs |
|---|---|---|---|
| Keyboard (default) | keyboard, mouse | `0410` + board | yes |
| Dinput | keyboard, mouse, 2 gamepads | `0412` + board | yes |
| Xinput | Xbox 360 controller | `045e:028e` | no |

In Xinput the board leaves the `d209` space entirely and publishes no LED interface. Reset to keyboard by holding P1SW1 while connecting USB.

`PID = 0x0410 + (dinput ? 2 : 0) + (boardId - 1)`. Board 2 needs Ultimarc's "ID #2" firmware. `0413` is inferred from the pattern; `0410`, `0411` and `0412` are observed.

### Interfaces

The LED collection is the last interface. How many precede it depends on the mode, not the firmware version.

| Mode | Interfaces | LED interface |
|---|---|---|
| Keyboard, or 1.4x firmware | 3 | 2 |
| Dinput, or 1.3x firmware | 4 | 3 |

Confirmed by `lsusb -v` on a keyboard mode board: three HID interfaces, 0 declaring boot subclass with protocol 1 (keyboard), 1 declaring boot subclass with protocol 2 (mouse), and 2 declaring subclass 0 with protocol 0, a 64 byte IN endpoint at `0x84` and a 136 byte report descriptor. Interface 2 is both the last one and the only one that is neither keyboard nor mouse, so either rule picks it. In Dinput the gamepad would also be subclass 0, which is why the position rule is the one used.

Note that interface 2 publishes **no OUT endpoint**. LED writes go through control transfers on endpoint 0, which is why `USB::send()` works without ever touching the interface's own endpoint.

`bcdDevice & 0x40` was used for this and is wrong. The bit only distinguishes the split firmware naming, where the middle digit is `3` with a gamepad and `4` without. It cannot see a runtime mode switch, so on Multi-Mode firmware it always answers "no gamepad", which is false in Dinput mode. Ultimarc-linux carries the same bug; QtPyUltimarc hardcodes interface 2 and does not support Dinput.

The version the board reports is not the version the firmware notes use. A board running current firmware reported `bcdDevice 0.49`, not a `1.5x` value. `0x49 & 0x40` is set, so the old test still landed on interface 2 in keyboard mode, by luck rather than by meaning. Do not map `bcdDevice` onto the firmware table below.

### Firmware history

| Versions | Behaviour |
|---|---|
| 1.22-1.33 | keyboard only, single firmware |
| 1.34-1.39 | keyboard with gamepad, always both, no Xinput, incompatible with RetroPie. Still offered as alternative firmware |
| 1.44-1.49 | keyboard and mouse, no gamepad |
| 1.50-1.56 | Multi-Mode, one device type at a time |
| 1.57 | adds any-button mode switching, not on all boards |

Shift functionality must be enabled before flashing Multi-Mode or mode switching will not work. Default shift control is Start1.

### LED protocols

Two HID collections, distinguished by usage page. Both can exist on one board; the SDK's `RemoveNewDriver()` merges them by matching product ID, and takes the fast path only when the merge produced a pointer.

| | Old collection | Fast collection |
|---|---|---|
| Usage page, usage | 1, 0 | 6, 0 |
| Output report length | non-zero | 97 |
| Report ID | 3 | 4 |
| Full update | `0xFE` then 48 pairs | one packet, 96 bytes |

Usage 0 on page 1 is what separates the LED collection from keyboard (usage 6), mouse (2) and gamepad (5). Reading the report descriptor of the LED interface answers both which interface and which protocol.

The 5 byte message, agreed by all three reference implementations:

| Byte | Content |
|---|---|
| 0 | `0x03`, the report ID |
| 1 | command |
| 2 | value |
| 3-4 | reserved, zero |

Commands match PacLed64 (`0x80` all, `0x89` random, `0xC0` fade rate, `0xFE` stream start, otherwise the LED index).

`IPAC_ULTIMATE_WVALUE` stays `0x0203` on the fast path even though the payload's report ID is `0x04`. That combination has shipped and works. If a board detects the fast collection but stays dark, try `0x0204`.

### Configuration channel

Not implemented here, documented by Ultimarc-linux. Pin assignments, macros and debounce are written to the same interface with a 260 byte payload prefixed `0x50 0xdd 0x0f 0x00`, and read back with `0x59 0xdd 0x0f 0x00`. `IPACULTIMATE_DATA_SIZE` is 260. Board ID is also set this way, which is how the "ID #2" firmware is applied.

## ServoStik

Two-position gate, 4 way or 8 way only.

| Property | Value |
|---|---|
| PID | `1700`, fixed |
| Interface | 0 |
| `wValue` | `0x0200` |
| Message | `{0x00, 0xdd, 0x00, ways}` where ways is 0 for 4 way and 1 for 8 way |
| Max players | 2 |

The PID never changes, so multiple boards are told apart by bus and port position (`isNonBasedId()`). One command moves both restrictors on a board, so the first player's setting wins and the rest are ignored.

The board cannot be queried for its current position. LEDSpicer caches the last value in `~/.config/ledspicer/servostik/last_<profile>.dat` to skip redundant moves. Deleting the file forces the next rotation.

## UltraStik360

Analog joystick with a downloadable 9x9 restrictor map.

| Property | Value |
|---|---|
| PID | `0511 + boardId - 1` (2015+), `0501` pre 2015 |
| Interface | 2 (2015+), 0 pre 2015 |
| `wValue` | `0x0200` |
| Transfer | 96 bytes in 24 chunks of 4 |

Payload layout:

| Byte | Content |
|---|---|
| 0 | keep analog: `0x50` off, `0x11` on (2015+) |
| 1 | map size, 9 |
| 2 | mechanical restrictor: `0x10` off, `0x09` on |
| 3-10 | cell borders, 8 values |
| 11-92 | map data, 9 rows of 9 |
| 95 | pre 2015 only: `0xFF` RAM, `0x00` flash. Must be 0 on 2015+ |

Maps come from `.um` files in `PROJECT_DATA_DIR/umaps/`. Default borders are `{30,58,86,114,142,170,198,226}`. Parsed maps are cached per `Ways` value for the process lifetime.

Pre 2015 boards are not supported: the PID and the interface both differ and nothing selects between them.

Confirmed by `lsusb -v`: player 1 enumerates as `d209:0511` and player 2 as `d209:0512`, so the PID increment is real, and each publishes three HID interfaces with the control interface at number 2, matching `ULTRASTIK_INTERFACE`.

## Known gaps

- PacDrive byte 1 differs from the SDK, see above.
- PacLed64 and NanoLed claim interface 0 while their headers name interface 1.
- Pre 2015 UltraStik360 unreachable.
- U-HID and USB Button are in the Ultimarc HID space and unimplemented.
- Ultimate `0413` unobserved.
- Whether "ID #2" board firmware and Multi-Mode mode switching combine is undocumented in every source checked.
