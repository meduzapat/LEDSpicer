# LEDSpicer RPM spec file
# Maintainer: Patricio A. Rossi (MeduZa) <meduzapat@netscape.net>
# https://github.com/meduzapat/LEDSpicer

%global debug_package %{nil}

Name:           ledspicer
Version:        0.7.3
Release:        1%{?dist}
Summary:        LED controller daemon for arcade cabinets and RGB lighting
License:        GPL-3.0-or-later
URL:            https://github.com/meduzapat/LEDSpicer
Source0:        %{url}/archive/refs/tags/%{version}.tar.gz#/LEDSpicer-%{version}.tar.gz

BuildRequires:  cmake >= 3.10
BuildRequires:  gcc-c++
BuildRequires:  pkgconf
BuildRequires:  tinyxml2-devel >= 6.0
BuildRequires:  libusb1-devel >= 1.0.22
BuildRequires:  pulseaudio-libs-devel >= 0.9
BuildRequires:  alsa-lib-devel >= 0.2
BuildRequires:  systemd-rpm-macros

Requires:       tinyxml2 >= 6.0
Requires:       libusb1 >= 1.0.22
Requires:       pulseaudio-libs >= 0.9
Requires:       alsa-lib >= 0.2

Requires:       tinyxml2 >= 6.0
Requires:       libusb1 >= 1.0.22
Requires:       pulseaudio-libs >= 0.9
Requires:       alsa-lib >= 0.2

# Note: RaspberryPi plugin not available on Fedora (pigpio not packaged)

%description
LEDSpicer is a robust linear LED controller daemon engineered to manage
both single-color and RGB LEDs across a wide range of devices.

This solution enables the seamless execution of sophisticated animations
and profiles, delivering a visually striking and dynamic user experience.

Install device plugin packages separately based on your hardware.

%post
%systemd_post ledspicerd.service processlookup.service

%preun
%systemd_preun ledspicerd.service processlookup.service

%postun
%systemd_postun_with_restart ledspicerd.service processlookup.service

# =============================================================================
# Device Plugin Subpackages
# =============================================================================

%package        nanoled
Summary:        LEDSpicer plugin for Ultimarc NanoLed
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description    nanoled
LEDSpicer device plugin for Ultimarc NanoLed LED controllers.

%package        pacdrive
Summary:        LEDSpicer plugin for Ultimarc PacDrive
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description    pacdrive
LEDSpicer device plugin for Ultimarc PacDrive LED controllers.

%package        pacled64
Summary:        LEDSpicer plugin for Ultimarc PacLed64
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description    pacled64
LEDSpicer device plugin for Ultimarc PacLed64 LED controllers.

%package        ultimateio
Summary:        LEDSpicer plugin for Ultimarc Ultimate I/O
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description    ultimateio
LEDSpicer device plugin for Ultimarc Ultimate I/O boards.

%package        ledwiz32
Summary:        LEDSpicer plugin for Groovy Game Gear LedWiz32
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description    ledwiz32
LEDSpicer device plugin for Groovy Game Gear LedWiz32 LED controllers.

%package        howler
Summary:        LEDSpicer plugin for WolfWareTech Howler
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description    howler
LEDSpicer device plugin for WolfWareTech Howler LED controllers.

%package        adalight
Summary:        LEDSpicer plugin for Adalight serial LEDs
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description    adalight
LEDSpicer device plugin for Adalight-compatible serial LED strips.

# Note: RaspberryPi plugin not available on Fedora (pigpio not packaged)
# For RaspberryPi support, use Arch Linux ARM or Debian-based distros

# =============================================================================
# Development Package
# =============================================================================

%package        devel
Summary:        Development files for %{name}
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description    devel
Development headers and pkg-config file for building applications
that use the LEDSpicer library.

# =============================================================================
# Build
# =============================================================================

%prep
%autosetup -n LEDSpicer-%{version}

%build
%cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-g0 -O3 -fPIC" \
    -DENABLE_PULSEAUDIO=ON \
    -DENABLE_ALSAAUDIO=ON \
    -DENABLE_NANOLED=ON \
    -DENABLE_PACDRIVE=ON \
    -DENABLE_PACLED64=ON \
    -DENABLE_ULTIMATEIO=ON \
    -DENABLE_LEDWIZ32=ON \
    -DENABLE_HOWLER=ON \
    -DENABLE_ADALIGHT=ON \
    %{nil}

%cmake_build

%install
%cmake_install

# =============================================================================
# File Lists
# =============================================================================

%files
%license %{_docdir}/ledspicer/COPYING
%doc %{_docdir}/ledspicer/README.md
%doc %{_docdir}/ledspicer/examples/
%{_bindir}/ledspicerd
%{_bindir}/emitter
%{_bindir}/rotator
%{_bindir}/inputseeker
%{_bindir}/processLookup
%{_libdir}/libledspicer.so.*
%dir %{_libdir}/ledspicer
%dir %{_libdir}/ledspicer/devices
%{_datadir}/ledspicer/
%{_unitdir}/ledspicerd.service
%{_unitdir}/processlookup.service

%files nanoled
%{_libdir}/ledspicer/devices/UltimarcNanoLed.so

%files pacdrive
%{_libdir}/ledspicer/devices/UltimarcPacDrive.so

%files pacled64
%{_libdir}/ledspicer/devices/UltimarcPacLed64.so

%files ultimateio
%{_libdir}/ledspicer/devices/UltimarcUltimate.so

%files ledwiz32
%{_libdir}/ledspicer/devices/LedWiz32.so

%files howler
%{_libdir}/ledspicer/devices/Howler.so

%files adalight
%{_libdir}/ledspicer/devices/Adalight.so

%files devel
%{_includedir}/ledspicer/
%{_libdir}/libledspicer.so
%{_libdir}/pkgconfig/ledspicer.pc

# =============================================================================
# Changelog (generated by generate-changelogs.sh)
# =============================================================================

%changelog
* Wed Jan 28 2026 Patricio A. Rossi <meduzapat@netscape.net> - 0.7.3-1
- Added
  - Support for separate project directories (`-j/--project` and `-J/--projects-dir` CLI options).
  - `LayoutProperties` struct for layout handling.
  - Unit tests for core utilities and device handling.
- Fixed
  - Servostiks board works in multi-board setups.
  - Pulseaudio actor now reconnects automatically and avoids fatal failures.
- Changed
  - Centralized path handling ensures consistent trailing slashes.
  - Systemd service files improved: graceful shutdown, optional Pulseaudio support for daemons.
  - `Utility::getHomeDir()` now resolves the correct home directory after privilege drops.
