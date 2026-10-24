# Restrictors

Joystick gate and mapping hardware, driven by `rotator`, not by the daemon. Same plugin and connection model as devices: `Restrictor : Hardware`, then `RestrictorUSB : USB, Restrictor` or `RestrictorSerial : Serial, Restrictor`.

Vendor detail for the Ultimarc and GroovyGameGear units lives with the rest of that vendor's hardware:

| Hardware | Vendor | Document |
|---|---|---|
| ServoStik, UltraStik360 | Ultimarc | [`devices/Ultimarc/README.md`](../devices/Ultimarc/README.md) |
| GP-Wiz49, GP-Wiz40 RotoX | GroovyGameGear | [`devices/GroovyGameGear/README.md`](../devices/GroovyGameGear/README.md) |
| TOS428 | GRS | below |

## Ways

`Restrictor::Ways` is the full set of positions any restrictor can be asked for. Each board maps the ones it cannot reach onto something it can.

| Value | Meaning |
|---|---|
| `invalid` | no rotation, skip the board |
| `w2` | 2 way horizontal |
| `w2v` | 2 way vertical |
| `w4` | 4 way |
| `w4x` | 4 way diagonal |
| `w8` | 8 way |
| `w16` | 16 way |
| `w49` | 49 way progressive |
| `analog` | raw analog |
| `mouse` | mouse emulation |
| `rotary8` | 8 position rotary |
| `rotary12` | 12 position rotary |

The list stops at what hardware implements. MAME also knows the half variants, 3 way (half 4) and 5 way (half 8) among others, and a mapped board like the UltraStik360 could express them in a `.um` file. They are left out on purpose: every one of them is played acceptably on its full-way equivalent, and adding them would force every other board to fold them back anyway. Add one only when a board can do something with it that `w4` or `w8` cannot.

Coverage by board:

| Board | Positions it can actually reach |
|---|---|
| ServoStik | `w4`, `w8`. Everything narrower folds to `w4`, everything else to `w8` |
| UltraStik360 | any, through a downloaded 9x9 map file |
| GP-Wiz49 | `w2`, `w2v`, `w4`, `w4x`, `w8`, `w16`, `w49`, `analog`, `mouse` |
| GP-Wiz40 RotoX | `rotary8`, `rotary12` only |
| TOS428 | `w4`, `w8`. Everything narrower folds to `w4`, everything else to `w8` |

Only ServoStik, UltraStik360 and TOS428 move anything physically. The GP-Wiz boards change how the sensor is interpreted.

## Player addressing

`getMaxIds()` is how many restrictors one board controls.

| Board | Max IDs | Addressing |
|---|---|---|
| ServoStik | 2 | none; one command moves both, the first player's value wins |
| UltraStik360 | 1 | one board per stick, board ID selects the board |
| GP-Wiz49 | 1 | one board per stick |
| GP-Wiz40 RotoX | 2 | both rotors in one message, separate bits |
| TOS428 | 4 | player number in the command, 0 addresses all. Only player 1 is really independent, see below |

## TOS428 (GRS)

Serial gate restrictor, four outputs on one board.

| Property | Value |
|---|---|
| Transport | serial, `RestrictorSerial` |
| USB ID for detection | `2341/8036` |
| Port scan | `/dev/ttyUSB0`-`4` and `/dev/ttyACM0`-`4` |
| Command | ASCII `setway,<player>,<ways>` |
| Inter-command delay | 1000 us |
| Max IDs | 4 |

`<player>` is `'0'` to `'4'` and `<ways>` is `'4'` or `'8'`, both as ASCII digits. Player `'0'` sets every restrictor at once, and `rotate()` uses it whenever more than one player is being set and they all want the same position. Otherwise one command is sent per player.

`2341` is Arduino's vendor ID and `8036` is the Leonardo. Detection matches `PRODUCT=2341/8036` in the port's uevent, so any Leonardo-class board attached at the same time can be picked up by mistake. Set `port` explicitly when that is possible.

Only two gate positions exist in the firmware, so everything below 8 way arrives as 4 way.

### Full command set

Extracted from the vendor's own `libtos428.so` (`SteBuTOS/tos428config-raspi`). Only `setway` is used here.

| Command | Note |
|---|---|
| `getway,%i` | |
| `setway,%i,%s` | way is the string `4` or `8` |
| `getangle,%i,%s` | angle is per restrictor and per way |
| `setangle,%i,%s,%i` | fine tunes the gate travel |
| `getcolor,%s` / `setcolor,%s,%i,%i,%i` | RGB, the board has lighting |
| `getstartupway` / `setstartupway,%s` | power on position |
| `getpurpose,%i` / `setpurpose,%i,%s` | |
| `getscope,%i` / `setscope,%i,%i,%i,%i,%i` | |
| `getkeycodes,%i,%s` / `setkeycodes,%i,%s,%s,%s,%s` | the board can emit key codes |
| `dumpeeprom` | dumps stored configuration |

`setangle`, `setcolor`, `setstartupway` and `setkeycodes` are all unused by LEDSpicer and have no equivalent on any other restrictor.

### The board replies, LEDSpicer does not read it

`tos428Cmd` in the vendor library is write, `usleep`, read. The Go implementation reads too and treats any reply other than `ok` as a fatal error. `RestrictorSerial` never reads, so a rejected or misparsed command is silently discarded and `rotate()` reports success either way. This is the diagnostic channel for anything below.

### Only gate 1 is individually addressable

Confirmed on hardware with two gates connected, from both `rotator` and raw serial.

| Command | Reply | Effect |
|---|---|---|
| `setway,0,<ways>` | `ok` | every gate moves |
| `setway,all,<ways>` | `ok` | every gate moves |
| `setway,1,<ways>` | `ok` | gate 1 moves |
| `setway,2,<ways>` | `ok` | **nothing moves** |
| `setway,` 3, 4, a, b, c or d | `ok` | nothing moves |

Nothing addresses gate 2 alone. Every candidate name was tried, numeric and alphabetic. The board is reachable as a whole, or through gate 1, and no other way.

Nothing above the wire is at fault. The format matches `tos428SetWay` in the vendor's own `libtos428.so` byte for byte, `player_map` ids of 1 and 2 are correct and distinct, and `rotate()` picks its broadcast and per-player branches correctly. The board accepts the command and declines to act on it.

Whether that is a firmware defect or a stored per port setting is **not yet established**. The vendor library exposes two per port properties nothing else uses:

| Property | Values | Command |
|---|---|---|
| purpose | `restrictor` or `keyboard` | `getpurpose,%i` / `setpurpose,%i,%s` |
| scope | four values, one per gate | `getscope,%i` / `setscope,%i,%i,%i,%i,%i` |

`tos428GetScope` parses exactly four integers out of the reply, one per gate, which reads as a matrix of which gates a target drives. A port whose purpose is `keyboard`, or whose scope selects no gate, would accept `setway` and move nothing, exactly as observed. Compare `getpurpose,1` against `getpurpose,2` and `getscope,1` against `getscope,2` before calling this a firmware bug.

**The board answers `ok` to a command it does not execute.** Reading the reply, which LEDSpicer does not do, would not have caught this. Reply checking is a diagnostic aid, not a fix.

Consequence: per-player commands cannot drive a cabinet that needs different positions at once. The all-same case is unaffected, because `rotate()` collapses it to one broadcast.

### Working around it

Two gates can still be set independently by ordering the writes so the broadcast lands first and gate 1 is corrected afterwards:

```
setway,0,<way wanted by gates 2 and up>
setway,1,<way wanted by gate 1>
```

Gate 1 moves twice and ends correct, every other gate moves once. This is right for two gates always, and for three or four whenever gates 2 and up agree with each other. It cannot help when three or more gates want three different positions, which the hardware cannot express at all.

`rotate()` does not do this yet.

### getway does not report position

`getway,<n>` returned `8` for every gate at every point during testing, including immediately after a gate had physically moved to 4. Whatever it reports, it is not live position, so it cannot be used to confirm a move or to skip a redundant one.

### Position is tracked in firmware

The board remembers where each gate is and ignores a command asking for the position it already holds. So the redundant-move problem that ServoStik solves with a cache file does not exist here, and no state is kept on the host. It also means a gate moved by hand, out of band, will stay wrong until something asks for a different position.

## Cross-cutting notes

- No restrictor reports its current position back to the host. ServoStik caches it in `~/.config/ledspicer/servostik/last_<profile>.dat` to skip redundant moves; TOS428 tracks it in firmware and needs nothing. Every other board is written unconditionally on each profile change.
- `boardId` is user-declared and matched to physical wiring, exactly as for devices. ServoStik is the exception: its PID is fixed, so boards are told apart by bus and port position through `isNonBasedId()`.
- Rotation happens on profile change, not per frame. The serial boards are slow enough that per frame would not work.
