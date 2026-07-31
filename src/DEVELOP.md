# DEVELOP mode reference

`-DENABLE_DEVELOP=ON` defines `DEVELOP=1` (CMakeLists.txt:53-62). The flag exists so instrumentation that is too noisy, too slow, or too invasive to ship can live in the tree without ever reaching a production binary. It is reserved for cases where step-by-step debugging cannot isolate the problem, typically per-frame state in the animation loop. It is a developer-only switch: nothing in the user documentation, the config format, or the runtime interface refers to it.

This document is the inventory of what the flag currently does, and the defects found while taking that inventory.

Line numbers are as of commit `a66c58b`.

## Summary

53 conditional sites across 22 files (52 `#ifdef DEVELOP`, 1 `#ifndef DEVELOP`).

| kind | count | what it means |
|---|---|---|
| Pure logging | 41 | adds output, changes nothing else |
| Behavioral | 6 | changes what the program does |
| Declaration / plumbing | 6 | includes, declarations, callback registration |

Those 41 logging sites are split across two channels that behave very differently, and the split is not visible at the call site:

| channel | sites | destination | level-gated |
|---|---|---|---|
| `LogDebug(...)` | 23 | syslog when daemonized, stdout otherwise | yes, by the macro itself |
| raw `cout`, wrapped in `if (Log::isLogging(LOG_DEBUG))` | 13 | stdout only | yes, manually |
| raw `cout`, unwrapped | 5 | stdout only | no |

The animation instrumentation, which is the reason the flag exists, is almost entirely on the raw `cout` channel.

## How actor debug output composes

Actor debug output is **additive**: each class in the call chain contributes its own part of one line, and the deepest one closes it. A frame line is assembled, not printed by a single owner. That is the design, and the absence of `endl` in the upper layers is correct.

Two shapes exist in the tree:

| shape | actors | line ends where |
|---|---|---|
| additive | `Serpentine`, `Filler` | `StepActor::changeFrameElement*` → `StepActor.cpp:34` / `StepActor.cpp:88` |
| standalone | `Gradient`, `Random`, `Pulse`, `FileReader`, `AudioActor` | own `endl` |

`Serpentine` and `Filler` are the only `StepActor` subclasses that call `changeFrameElement()`, so they are the only two participants in the additive chain.

The rule that follows: **`changeFrameElement()` terminates the line, `changeElementColor()` does not.** `changeElementColor()` contributes nothing to the chain. An actor that has opened a fragment and then reaches `changeElementColor()` ends the frame with the line still open.

## Bugs

### Serpentine: line abruptly ended without `endl` (known issue)

`Serpentine::calculateElements` opens its fragment at Serpentine.cpp:72 and then, on the `speed == VeryFast` shortcut, calls `changeElementColor()` (Serpentine.cpp:81) instead of `changeFrameElement()`. The chain is abandoned mid-line and nothing terminates it.

| path | ends the line |
|---|---|
| no tail, not VeryFast | `changeFrameElement` → StepActor.cpp:88 |
| no tail, **VeryFast** (Serpentine.cpp:80-81) | **nothing** |
| tail, not VeryFast | `changeFrameElement` → StepActor.cpp:88 |
| tail, VeryFast | explicit `endl` at Serpentine.cpp:127 |

The last row is the same defect already spot-fixed: the `endl` at Serpentine.cpp:125-129 was added precisely because that `VeryFast` branch leaves the chain too. The tail-less branch was missed.

With `tailLength="0" speed="VeryFast"` the daemon emits `Serpentine: → ` once per frame onto a single line that grows without bound. Measured on a 6-element group at 30 FPS: 522 characters after 3 seconds.

### Filler: the same defect, in all four modes, none of them fixed

Filler.cpp:71-87 opens `Filler: <dir> <Filling|Emptying> <color> `, correctly additive. Every mode then takes the same `speed == VeryFast` shortcut out of the chain, and unlike Serpentine none of them carries the compensating `endl`:

| mode | leaves the chain at |
|---|---|
| `fillElementsLinear` | Filler.cpp:116-117 |
| `fillElementsRandom` | Filler.cpp:145-146 |
| `fillElementsWave` | Filler.cpp:191-192 |
| `fillElementsCurtain` | Filler.cpp:208-211 |

Measured with `mode="Normal" speed="VeryFast"`: 779 characters on one line after 3 seconds.

Five broken paths in total, one shared cause: the `VeryFast` shortcut is the only place either actor calls `changeElementColor()` after opening a fragment.

### `Main.cpp:402` is a no-op

```cpp
Log::logToStdTerm(DataLoader::getMode() != DataLoader::Modes::Normal);
```

`Log::initialize()` was already called at Main.cpp:371 with the identical expression, and `Log::initialize` does nothing but forward to `logToStdTerm` (Log.cpp:32-34). `DataLoader::setMode()` is only ever called during argv parsing (Main.cpp:333-364), which completes before that. The line cannot change anything.

### DEVELOP `cout` output is discarded in the default run mode

Under DEVELOP, Main.cpp:403 forces `LOG_DEBUG`, but `Modes::Normal` still daemonizes at MainBase.cpp:46 (`daemon(0, 0)`), which points stdout at `/dev/null`. `LogDebug` survives, because `logToStdTerm(false)` switched the logger to syslog. Every raw-`cout` site — which is most of the instrumentation — is lost. A DEVELOP build only shows its animation output when run with `-f`.

`DRY_RUN` masks this: the daemonize call is inside `#ifndef DRY_RUN`, so a DEVELOP+DRY_RUN build never daemonizes and everything appears. DEVELOP alone does not behave the same way.

### The `isLogging(LOG_DEBUG)` guards are dead in the daemon

Main.cpp:403 forces the log level to `LOG_DEBUG` unconditionally in a DEVELOP build, so `if (Log::isLogging(LOG_DEBUG))` is always true inside `ledspicerd`. The guard's only real effect is in the gtest binaries, which never raise the level — so what looks like a runtime switch for the developer is in practice accidental noise suppression for tests. Verified: test output is byte-identical between DEVELOP=ON and DEVELOP=OFF builds.

### Five raw `cout` sites bypass that guard

Actor.cpp:148, FileReader.cpp:119, FileReader.cpp:123, Profile.cpp:31 and Profile.cpp:36 write to `cout` with no level check at all. They are inconsistent with the other thirteen raw-`cout` sites, and they are the sites that would leak into test output.

### `DataLoader.cpp:785` logs in the wrong place

`LogDebug("Transitions deleted")` sits after the profile-destruction loop, but the transitions are destroyed at DataLoader.cpp:774-776, before it. It also sits at two tabs inside a one-tab function body.

### `Device::getLed`'s DEVELOP validation is redundant

Device.cpp:60 calls `validateLed(ledPos)`; Device.cpp:62 returns `&LEDs.at(ledPos)`, which already bounds-checks in every build. The only effect of the DEVELOP block is to substitute `LEDSpicer::Error` for `std::out_of_range`. The matching block in `setLed` (Device.cpp:47) is *not* redundant — that one uses `LEDs[led]`.

### `AudioActor::displayPeak()` is one frame late

`refreshPeak()` (AudioActor.cpp:95-101) calls `displayPeak()` before resetting `value` and calling `calcPeak()`. The printed peak is always the previous frame's, never the one about to be drawn.

### `FileReader.cpp:85-89` hides a user-facing validation

```cpp
#ifdef DEVELOP
	if (ceil > getNumberOfElements()) {
		LogWarning("More colors than element colors detected");
	}
#endif
```

That is a malformed-animation-file warning — a config error an end user can produce and would want reported. `LogWarning("Empty Frame detected")` six lines above (FileReader.cpp:79) is the same class of check and is not gated. This one is invisible in production builds.

### Duplicated debug block

FrameActor.cpp:131-134 and DirectionActor.cpp:116-119 are identical, because `DirectionActor::advanceFrame` overrides `FrameActor::advanceFrame` and re-implements the cycle counting alongside it.

### Indentation

Main.cpp:401-403, Serpentine.cpp:115-119, Serpentine.cpp:126-128 and DataLoader.cpp:786 sit at the wrong tab depth for their enclosing scope. Filler.cpp:75-83 nests an `if`/`else` inside what reads as a continuing stream expression.

### Inconsistent gating of equivalent messages in PulseAudio

`PA_CONTEXT_CONNECTING` is DEVELOP-only (PulseAudio.cpp:174) while its siblings `PA_CONTEXT_READY` and `PA_CONTEXT_TERMINATED` log unconditionally (PulseAudio.cpp:184, 209). The stream-state switch has the same split: `PA_STREAM_CREATING` and `PA_STREAM_TERMINATED` are DEVELOP-only (PulseAudio.cpp:293-301), `PA_STREAM_READY` and `PA_STREAM_FAILED` are not.

## Not bugs, but worth recording

- **The missing `endl` in the `Serpentine` and `Filler` openers is correct.** They are the top of an additive chain, not standalone lines. The fix for the five broken paths is to keep the chain intact (or close it at the point it is left), never to add `endl` at Serpentine.cpp:72 or Filler.cpp:71 — that would split every working frame line in two.
- **`MainBase.cpp:91` is the only `#ifndef DEVELOP` in the tree, and it is correct.** It skips `validateLed()` in `testLeds` precisely because `Device::setLed` gains that validation under DEVELOP (Device.cpp:46). Correct, but it is an invisible coupling between two files.
- **DEVELOP compiles clean.** Verified against DRY_RUN on/off, BENCHMARK, SHOW_OUTPUT, MiSTer, TESTS, both audio actors, and every USB/serial device plugin. No warnings, no dead references.
- **`tests/` contains no DEVELOP references.** The flag is untested by construction.
- **No user-facing document mentions DEVELOP,** which is correct.

## Full site table

Channel legend: `L` = `LogDebug`, `C` = raw `cout` behind `isLogging(LOG_DEBUG)`, `C!` = raw `cout`, unguarded, `B` = behavioral, `D` = declaration or plumbing.

### Build

| file:line | ch | what |
|---|---|---|
| CMakeLists.txt:53 | | `option(ENABLE_DEVELOP "Enables development mode" OFF)` |
| CMakeLists.txt:59-62 | | `add_compile_definitions(DEVELOP=1)` |
| CMakeLists.txt:477 | | prints `Develop mode` in the configure summary |

### Core

| file:line | ch | what | notes |
|---|---|---|---|
| Main.cpp:26-28 | D | `#include <execinfo.h>` | |
| Main.cpp:52-58 | B | backtrace dump on SIGSEGV / SIGILL / SIGFPE / SIGBUS | the flag's clearest justified use |
| Main.cpp:400-404 | B | force `logToStdTerm()` + `setLogLevel(LOG_DEBUG)` | line 402 is a no-op; line 403 kills every `isLogging` guard |
| MainBase.cpp:59-61 | L | "Device Handler of type X instance deleted" | |
| MainBase.cpp:91-93 | B | `#ifndef` — skip `validateLed` in `testLeds` | only inverted use; pairs with Device.cpp:46 |
| DataLoader.cpp:779-781 | L | "Profile X instance deleted" | |
| DataLoader.cpp:785-787 | L | "Transitions deleted" | wrong position, wrong indent |
| Socks.cpp:105-107 | L | "Message sent: [...]" | shared lib, also used by emitter/rotator |
| Socks.cpp:140-142 | L | "Message received: [...]" | shared lib |
| Network.cpp:54-56 | L | "Processing \<entry\>" | Network.cpp:50 already logs the whole buffer, ungated |

### Devices

| file:line | ch | what | notes |
|---|---|---|---|
| Device.cpp:46-48 | B | `validateLed()` in `setLed` | meaningful — `setLed` uses `LEDs[led]` |
| Device.cpp:59-61 | B | `validateLed()` in `getLed` | redundant — `getLed` returns `LEDs.at()` |
| DeviceHandler.cpp:43-45 | L | "\<device\> instance deleted" | |
| Profile.cpp:31-33 | C! | "Removing profile actors" | unguarded |
| Profile.cpp:36-38 | C! | "Removing profile inputs" | unguarded |

### Actor tree, base classes

| file:line | ch | what | notes |
|---|---|---|---|
| Actor.cpp:57-59 | L | "Actor N Initiated" | |
| Actor.cpp:122-124 | L | "Actor N restarting" | |
| Actor.cpp:148-158 | C! | per-actor timer state dump on `frame == 1` | unguarded |
| Actor.cpp:168-170 | L | "Starting Actor N by time after X seconds" | |
| Actor.cpp:183-185 | L | "Ended Actor N by time after X seconds" | |
| Actor.cpp:193-195 | L | "Repeat consumed for actor N" | |
| Actor.cpp:202-204 | L | "Restart timer set on actor N" | |
| Actor.cpp:217-219 | L | "Restarting Actor N after X seconds" | |
| FrameActor.cpp:75-77 | L | "Starting ActorN from frame X" | missing space after `Actor` |
| FrameActor.cpp:131-134 | L | "Actor N completed after X cycles" | duplicated in DirectionActor |
| DirectionActor.cpp:116-119 | L | "Actor N completed after X cycles" | duplicate of the above |
| StepActor.cpp:32-36 | C | "Element N faded X%" + `endl` | deepest layer of the additive chain, terminates the line |
| StepActor.cpp:81-90 | C | "Frame X to Y Step S to T P%" + `endl` | deepest layer of the additive chain, terminates the line |

### Actor plugins

| file:line | ch | what | notes |
|---|---|---|---|
| Serpentine.cpp:72-76 | C | `"Serpentine: <dir> "` | opens the fragment; additive, correct |
| Serpentine.cpp:88-92 | C | `"Tail: "` | appends to the fragment |
| Serpentine.cpp:95-99 | C | `"<pos>=--% "` — tail element on the head position | |
| Serpentine.cpp:114-120 | C | `"<pos>=<pct>% "` | misindented |
| Serpentine.cpp:125-129 | C | `endl`, tail + VeryFast path only | spot-fix for one `VeryFast` branch leaving the chain; misindented |
| Filler.cpp:71-87 | C | `"Filler: <dir> <Filling\|Emptying> <color> "` | opens the fragment; additive, correct |
| Gradient.cpp:59-68 | C | direction + frame/step counters | self-terminating |
| Random.cpp:39-43 | C | frame + transition percent | self-terminating |
| Pulse.cpp:42-50 | C | frame, fade percent, colour | self-terminating |
| FileReader.cpp:85-89 | B | `LogWarning` on more colours than elements | user-facing validation, wrongly gated |
| FileReader.cpp:119-121 | C! | `"FileReader: <dir> F: n"` | unguarded |
| FileReader.cpp:123-127 | C! | per-element hex dump | unguarded, one line per element per frame |

### Audio actors

| file:line | ch | what | notes |
|---|---|---|---|
| AudioActor.hpp:130-135 | D | declares `displayPeak()` | |
| AudioActor.cpp:68-75 | C | element totals at construction | |
| AudioActor.cpp:96-98 | D | calls `displayPeak()` from `refreshPeak()` | prints the previous frame's peak |
| AudioActor.cpp:103-111 | D | defines `displayPeak()` | |
| AudioActor.cpp:325-333 | C | per-element channel / value / index dump in `waves()` | |
| PulseAudio.cpp:174-176 | L | "Connecting context" | siblings log ungated |
| PulseAudio.cpp:197-199 | L | "Context event changed" | |
| PulseAudio.cpp:228-230 | L | "Finding monitor for \<source\>" | |
| PulseAudio.cpp:233-235 | L | "Not found" | |
| PulseAudio.cpp:240-249 | L | sink description, index, sample spec, channel map, frame/sample size | also declares two local buffers |
| PulseAudio.cpp:262-277 | D | registers underflow / overflow stream callbacks | effect is logging only |
| PulseAudio.cpp:293-301 | D | adds `PA_STREAM_CREATING` / `PA_STREAM_TERMINATED` switch cases | fall into `default` otherwise |
| AlsaAudio.cpp:118-120 | L | "ALSA underrun, recovering" | |

### Documentation

| file:line | what |
|---|---|
| src/README.md:201-210 | how to use conditional compilation for development-only code |
| src/README.md:315-334 | lists `DEVELOP` among the CMake-provided macros |
