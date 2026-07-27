# GroovyGameGear

USB vendor ID `0xFAFA`.

| Product | PID | Boards | Implemented here |
|---|---|---|---|
| LED-Wiz 32 | `00F0` + boardId - 1 | 16 | `LedWiz32.{hpp,cpp}` |
| GP-Wiz49 | `0007` + boardId - 1 | 4 | `restrictors/GPWiz49.{hpp,cpp}` |
| GP-Wiz40 RotoX | `0035` + boardId - 1 | 4 boards, 2 rotors each | `restrictors/GPWiz40RotoX.{hpp,cpp}` |

The LED-Wiz vendor and product IDs are confirmed by the Pinscape controller and by Arduino LED-Wiz clones, which reuse `fafa:00f0` to be picked up by existing software. The GP-Wiz codes come from this project's source and were not confirmed against an external reference.

## LED-Wiz 32

32 outputs with PWM dimming. Not RGB; each output is one channel.

| Property | Value |
|---|---|
| Interface | 0 |
| `wValue` | `0x0200` (report ID 0) |
| Message | 8 bytes |
| Inter-message delay | 1000 us |

Two message types, told apart by the first byte.

### SBA, first byte 64

Enables outputs and sets the global pulse speed.

| Byte | Content |
|---|---|
| 0 | `64` |
| 1-4 | on/off bitmasks for outputs 1-32, one bit per output |
| 5 | global pulse speed, 1 to 7 |
| 6-7 | zero |

Sent once in `afterClaimInterface()` as `{64, 255, 255, 255, 255, 1, 0, 0}`: every output enabled, fastest pulse. This also initialises the board's four controller ICs.

### PBA, any other first byte

Sets the per-output profile. 32 values sent as 4 messages of 8 bytes, in output order.

| Value | Meaning |
|---|---|
| 0 | off |
| 1-48 | PWM duty, 48 is maximum |
| 49 | solid on, not PWM |
| 129-132 | automated pulse modes, speed from the SBA global pulse speed |

LEDSpicer maps 0-255 onto 0-48 with `48 * (intensity / 255.0)`, so value 49 is never sent. The difference between 48 and 49 is PWM at full duty versus a steady line. Visually identical on most LEDs; 49 matters for non-LED loads driven through the outputs.

PBA is a profile, not an on/off control. Vendor documentation warns against using it for switching. LEDSpicer uses it as the only write path, having enabled everything once through SBA, which is the standard approach.

The 1000 us delay between messages is required. The board drops messages sent back to back.

Output order in `drawHardwareLedMap()` is a left column of outputs 1-16 and a right column of 32 down to 17.

## GP-Wiz49

49 way joystick with selectable output mapping.

| Property | Value |
|---|---|
| Interface | 0 |
| `wValue` | `0x0200` |
| Message | `{204, mode}` |

| Mode | Meaning | LEDSpicer `Ways` |
|---|---|---|
| 1 | 49 way, raw | `analog`, `mouse` |
| 2 | progressive 49 | `w49` |
| 3 | 8 way | default |
| 4 | 4 way | `w4` |
| 5 | 4 way diagonal | `w4x` |
| 6 | 2 way horizontal | `w2` |
| 7 | 2 way vertical | `w2v` |
| 8 | 16 way, large centre dead zone | `w16` |

Adding 10 to the mode selects the Williams Sinistar variant of the same mapping, enabled with the `williams` option. Modes 11-18 are therefore valid and are the documented way to drive a Sinistar-style 49 way stick.

This is a mapping change, not a mechanical one. Nothing moves; the board reinterprets the sensor.

## GP-Wiz40 RotoX

Two rotary sticks per board, addressed together in one message.

| Property | Value |
|---|---|
| Interface | 0 |
| `wValue` | `0x0200` |
| Message | `{207, speedOn, speedOff, flags}` |
| Rotors per board | 2 |

Flags byte:

| Bit | Value | Meaning |
|---|---|---|
| 0 | 1 | 12 way rotary on stick 1 |
| 1 | 2 | 12 way rotary on stick 2 |
| 4 | 16 | 8 way rotary on stick 1 |
| 5 | 32 | 8 way rotary on stick 2 |

Both rotors are set in a single write. Nothing is sent when the flags byte is zero, that is when no configured player asked for a rotary mode.

`speedOn` and `speedOff` control the rotation timing and are members of the class rather than derived from the profile.

Only `rotary8` and `rotary12` reach this board. Any other `Ways` value leaves the corresponding rotor untouched.

## Known gaps

- GP-Wiz vendor and product IDs unconfirmed outside this source tree.
- LED-Wiz value 49 and the 129-132 pulse modes are unreachable through LEDSpicer.
- The LED-Wiz SBA global pulse speed is fixed at 1 and not configurable.
- `speedOn` and `speedOff` on the RotoX are not exposed in the config.
