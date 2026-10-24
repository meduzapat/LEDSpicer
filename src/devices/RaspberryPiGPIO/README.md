# Raspberry Pi GPIO

The host's own GPIO header, driven through [pigpio](https://abyz.me.uk/rpi/pigpio/). No USB, no serial, no connection at all: `RaspberryPi` derives from `Device` directly rather than `DeviceUSB` or `DeviceSerial`.

Built only when `ENABLE_RASPBERRYPI` is on, which also requires `libpigpio`.

| Property | Value |
|---|---|
| Channels | 28 (BCM GPIO 1 to 28) |
| Dimming | software PWM, 0 to 255 |
| Transfer mode | `RPI_TRANSFER 1`, individual |
| Board ID | none |

## Addressing

LEDSpicer LED index `n` maps to BCM GPIO `n + 1`. The offset comes from `openHardware()`, which computes each element's GPIO from its position in the buffer:

```
gpioled = led - firstled + 1
```

So LED 0 is GPIO 1, LED 27 is GPIO 28. GPIO 0 is not reachable.

## Behaviour

`openHardware()` walks the registered elements and calls `gpioSetMode(pin, PI_OUTPUT)` only for pins an element actually claims, collecting them into `usedleds`. `transfer()` then calls `gpioPWM()` for those pins alone. Unassigned pins are never touched and keep whatever mode the system gave them, which is what makes it safe to share the header with other functions.

`resetLeds()` writes PWM 0 to the used pins and clears the buffer.

## Single instance

`gpioInitialise()` is process wide, so the class guards it with a static `initialized` flag and throws on a second instance:

```
<name> device can only be loaded once
```

Two Raspberry Pi devices in one config is a configuration error, not a supported multi-board setup.

## Constraints

- pigpio requires root, or the pigpio daemon with appropriate permissions. LEDSpicer drops privileges after devices are initialised, so initialisation must happen while still root.
- Software PWM on many pins costs CPU. There is no hardware PWM path here; `gpioPWM()` is used for every pin regardless of whether it is one of the pins capable of hardware PWM.
- No current limiting. Pins source a few milliamps and need a driver stage for anything brighter than an indicator LED.
- Pin numbering is BCM, not physical header position. `drawHardwareLedMap()` prints the header layout with the ground and power positions marked `G` and `+`.

## Known gaps

- The constructor ignores its `options` argument entirely, so no per-board configuration reaches the class.
- No mapping from LED index to a chosen GPIO. The `+1` offset is fixed, so a config cannot skip pins reserved for I2C, SPI or UART; it can only leave them unassigned.
- Hardware PWM pins are not treated differently from the rest.
