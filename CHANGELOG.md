# Changelog

All notable changes to LEDSpicer will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.7.7] - 2026-06-30

### Added
- Automated release publishing to COPR and AUR.

### Changed
- Daemon starts independently of the user session and audio; audio now connects lazily so the service comes up as soon as the network is ready (#125)
- systemd service no longer waits on `sound.target` and no longer sets `PULSE_SERVER` — the daemon derives the per-user PulseAudio socket itself (#125)
- CMake build cleanups and definition fixes; generated `config.hpp`

### Fixed
- ALSA capture device opens without throwing and reopens automatically if it is missing or later lost (#125)
- PulseAudio falls back to the background retry on an initial connection failure instead of aborting startup (#125)
- Rotator argument-parsing crash and a redundant initialization (#123)
- Non-working TOS serial restrictor (#123)
- Hardened `colorFormat` parsing (accepts `RGB`/`rgb`) and now warns on duplicate player/joystick rotator requests instead of silently dropping them (#123)

## [0.7.6] - 2026-06-14

### Fixed
- Crash and cache handling for crafted profiles (#121)
- Use-after-free when loading same crafted profile twice
- Cache/retrieve crafted profiles by game name
- FORCE_RELOAD and REPLACE flags on craft/load path
- Profile stack history for FinishLastProfile

## [0.7.4.1] - 2026-05-06

### Fixed
- Several actor behaviors broken by the removal of `cycles` in 0.7.4
- `restartTime` causing inconsistent actor behavior
- `startAt` not working correctly when running in backward direction
- `repeat` not working correctly

## [0.7.4] - 2026-05-03

### Added
- `restartTime` parameter for actors: delay before restarting after `endTime` expires
- Time is now frame-based, syncing multiple actors to the same point in time

### Changed
- Time and duration values now accept decimals (e.g. `1.5` seconds)
- Repeat limit increased from 255 to 65535
- Improved actor runtime validation
- `checkRepeats()` infinite repeat behavior corrected

### Removed
- `cycles` parameter from FrameActor, replaced by `endTime`

### Fixed
- Serpentine animation debug log showed wrong position
- Element reset loop applied incorrectly in some cases

## [0.7.3] - 2026-01-28

### Added
- Support for separate project directories (`-j/--project` and `-J/--projects-dir` CLI options).
- `LayoutProperties` struct for layout handling.
- Unit tests for core utilities and device handling.

### Fixed
- Servostiks board works in multi-board setups.
- Pulseaudio actor now reconnects automatically and avoids fatal failures.

### Changed
- Centralized path handling ensures consistent trailing slashes.
- Systemd service files improved: graceful shutdown, optional Pulseaudio support for daemons.
- `Utility::getHomeDir()` now resolves the correct home directory after privilege drops.

## [0.7.2] - 2026-01-24

### Added
- Fixed isses with build
- Changed dir structure (tnx to debian)
- Updated docs

### Changed
- Removed Raspberry Pi GPIO plugin.
- Moved debian/ to repo root for Launchpad compatibility.

## [0.7.1] - 2026-01-18

### Added
- Multi-distribution packaging infrastructure (AUR, COPR, PPA)
- Automated changelog generator from CHANGELOG.md
- Split packages: base daemon + separate plugin per device

### Changed
- CMakeLists.txt now single source of truth for all packaging metadata

## [0.7.0] - 2026-01-14

### Added
- Unit test suite with 27 GTest tests and DRY_RUN mode
- New Filler animation curtain mode
- New flexible transition system architecture

### Changed
- Build system migrated from Autotools to CMake 3.10+
- Plugin restructure: animations and inputs now compiled into binary, only devices remain as plugins
- Code modernization to C++17 with extensive refactoring
- Direction system simplified to Forward/Backward + bouncer flag
- Updated PulseAudio plugin for PipeWire compatibility

### Fixed
- ALSA audio plugin
- Serial communication reliability

### Breaking Changes
- Data files now expect version 1.1
- Direction system API changed

## [0.5.4] - 2020-11-05

### Added
- Colors to Filler and Pulse animations
- ALSA audio plugin (experimental)

### Changed
- Better debug output for several actors

### Fixed
- PulseAudio issue

## [0.5.3] - 2020-10-06

### Added
- Animation files to transitions

### Changed
- Some mandatory parameters now optional

### Fixed
- Timing issue with ledwiz32
- Issue on nanoled

## [0.5.2] - 2020-09-07

### Added
- Process lookup feature

## [0.5.1] - 2020-08-29

### Added
- colors.ini color feature to emitter and ledspicer
- Pseudo colors On and Off

### Fixed
- Various issues

## [0.5.0] - 2020-08-19

### Added
- Support for GGG GP49 and GP40 + rotoX
