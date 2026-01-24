# LEDSpicer Distribution Packaging

This directory contains packaging files for distributing LEDSpicer across different Linux distributions.

## Single Source of Truth

Everything comes from `CMakeLists.txt`:

| Data | CMake Variable |
|------|----------------|
| **Version** | `project(LEDSpicer VERSION x.y.z)` |
| **URL** | `PROJECT_SITE` |
| **Data Version** | `DATA_VERSION` |
| **Maintainer** | `COPYRIGHT` (name parsed from end) |
| **Email** | `MAINTAINER_EMAIL` |

Changes are documented in `CHANGELOG.md` (Keep a Changelog format).

## Release Workflow

1. Update version in `CMakeLists.txt`
2. Update `CHANGELOG.md` with your changes
3. Run `./packaging/generate-changelogs.sh`
4. Review generated files
5. Commit, tag, push
6. Push to launchpad `git push launchpad master` (or the branch to use)

The script reads version from CMakeLists.txt and generates:
- `debian/changelog` - Debian format (uses `unstable`, not Ubuntu codenames)
- `arch/.SRCINFO` - AUR metadata
- Updates version in `PKGBUILD` and `.spec`
- RPM changelog entry (printed to console)

## Repository Structure

```
LEDSpicer/
├── CMakeLists.txt            ← All metadata here (version, url, maintainer, email)
├── CHANGELOG.md              ← Changes in Keep a Changelog format
├── debian/
│   ├── changelog         ← Generated
│   ├── control
│   ├── rules
│   ├── copyright
│   ├── source/format
│   └── *.install
├── src/
├── data/
├── tests/
└── packaging/
    ├── generate-changelogs.sh
    ├── arch/
    │   ├── PKGBUILD
    │   ├── ledspicer.install
    │   └── .SRCINFO          ← Generated
    └── rpm/
        └── ledspicer.spec
```

**Benefits of integration:**
- Version synced with git tags
- CI/CD can build packages automatically
- Contributors can fix packaging issues
- `CHANGELOG.md` at repo root is standard practice

## Package Structure

| Package | Contents |
|---------|----------|
| `ledspicer` | Daemon, utilities, library, data files, systemd service, udev rules |
| `ledspicer-nanoled` | Ultimarc NanoLed plugin (`UltimarcNanoLed.so`) |
| `ledspicer-pacdrive` | Ultimarc PacDrive plugin (`UltimarcPacDrive.so`) |
| `ledspicer-pacled64` | Ultimarc PacLed64 plugin (`UltimarcPacLed64.so`) |
| `ledspicer-ultimateio` | Ultimarc Ultimate I/O plugin (`UltimarcUltimate.so`) |
| `ledspicer-ledwiz32` | Groovy Game Gear LedWiz32 plugin (`LedWiz32.so`) |
| `ledspicer-howler` | WolfWareTech Howler plugin (`Howler.so`) |
| `ledspicer-adalight` | Adalight serial LED plugin (`Adalight.so`) |
| `ledspicer-raspberrypi` | Raspberry Pi GPIO plugin (`RaspberryPi.so`) - **ARM only, not builded due to missing pigpio library** |
| `ledspicer-dev` | Development headers (`*.hpp`), pkgconfig file |
| `ledspicer-doc` | Documentation and example configurations |
