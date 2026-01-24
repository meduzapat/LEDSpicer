# Changelog

All notable changes to LEDSpicer will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

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
