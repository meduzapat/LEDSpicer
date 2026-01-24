#!/bin/bash
# generate-changelogs.sh
# Generates distro-specific changelog files from CHANGELOG.md
#
# All metadata is read from CMakeLists.txt (single source of truth)
#
# Usage: ./generate-changelogs.sh

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="${SCRIPT_DIR}/.."

# =============================================================================
# Read all metadata from CMakeLists.txt (single source of truth)
# =============================================================================

CMAKE_FILE="${REPO_ROOT}/CMakeLists.txt"
if [[ ! -f "$CMAKE_FILE" ]]; then
	echo "Error: CMakeLists.txt not found at $CMAKE_FILE"
	exit 1
fi

# Parse values from CMakeLists.txt
VERSION=$(sed -n 's/.*project(LEDSpicer VERSION \([0-9.]*\).*/\1/p' "$CMAKE_FILE")
DATA_VERSION=$(sed -n 's/.*set(DATA_VERSION[[:space:]]*"\([0-9.]*\)".*/\1/p' "$CMAKE_FILE")
URL=$(sed -n 's/.*set(PROJECT_SITE[[:space:]]*"\([^"]*\)".*/\1/p' "$CMAKE_FILE")
EMAIL=$(sed -n 's/.*set(MAINTAINER_EMAIL[[:space:]]*"\([^"]*\)".*/\1/p' "$CMAKE_FILE")
# Extract maintainer name from COPYRIGHT line: "Copyright © 2018 - 2026 Patricio A. Rossi"
MAINTAINER=$(sed -n 's/.*set(COPYRIGHT[[:space:]]*"[^"]*[0-9]\{4\} \(.*\)").*/\1/p' "$CMAKE_FILE")

# Validate required values
if [[ -z "$VERSION" ]]; then
	echo "Error: Could not parse VERSION from CMakeLists.txt"
	echo "Expected: project(LEDSpicer VERSION x.y.z ...)"
	exit 1
fi

if [[ -z "$EMAIL" ]]; then
	echo "Error: Could not parse MAINTAINER_EMAIL from CMakeLists.txt"
	echo "Expected: set(MAINTAINER_EMAIL \"your@email.com\")"
	exit 1
fi

if [[ -z "$URL" ]]; then
	echo "Error: Could not parse PROJECT_SITE from CMakeLists.txt"
	exit 1
fi

if [[ -z "$MAINTAINER" ]]; then
	echo "Warning: Could not parse maintainer name from COPYRIGHT, using default"
	MAINTAINER="Patricio A. Rossi (MeduZa)"
fi

# Package release is always 1 (reset on each new version)
RELEASE="1"

DATE=$(date -R)

# =============================================================================
# Find CHANGELOG.md
# =============================================================================

if [[ -f "${REPO_ROOT}/CHANGELOG.md" ]]; then
	CHANGELOG_MD="${REPO_ROOT}/CHANGELOG.md"
elif [[ -f "${SCRIPT_DIR}/CHANGELOG.md" ]]; then
	CHANGELOG_MD="${SCRIPT_DIR}/CHANGELOG.md"
else
	echo "Error: CHANGELOG.md not found in repo root or packaging dir"
	exit 1
fi

# =============================================================================
# Functions
# =============================================================================

# Extract changelog entry for specific version from CHANGELOG.md
extract_changes() {
	local version="$1"
	awk -v ver="$version" '
		/^## \[/ {
			if (found) exit
			if ($0 ~ "\\[" ver "\\]") found=1
			next
		}
		found && /^### / {
			gsub(/^### /, "  * ")
			print
			next
		}
		found && /^- / {
			gsub(/^- /, "    - ")
			print
		}
	' "$CHANGELOG_MD"
}

# Generate Debian changelog
generate_debian() {
	local outfile="${REPO_ROOT}/debian/changelog"
	local changes
	changes=$(extract_changes "$VERSION")

	cat > "$outfile" << EOF
ledspicer (${VERSION}-${RELEASE}) unstable; urgency=medium

${changes}

 -- ${MAINTAINER} <${EMAIL}>  ${DATE}
EOF

	echo "Generated: $outfile"
}

# Generate Arch .SRCINFO
generate_srcinfo() {
	local outfile="${SCRIPT_DIR}/arch/.SRCINFO"

	cat > "$outfile" << EOF
pkgbase = ledspicer
	pkgdesc = LED controller daemon for arcade cabinets and RGB lighting
	pkgver = ${VERSION}
	pkgrel = ${RELEASE}
	url = ${URL}
	arch = x86_64
	arch = aarch64
	arch = armv7h
	license = GPL3
	makedepends = cmake>=3.10
	makedepends = gcc
	makedepends = pkg-config
	makedepends = tinyxml2>=6.0
	makedepends = libusb>=1.0.22
	makedepends = libpulse>=0.9
	makedepends = alsa-lib>=0.2
	source = ledspicer-${VERSION}.tar.gz::${URL}/archive/refs/tags/${VERSION}.tar.gz
	sha256sums = ${CHECKSUM}

pkgname = ledspicer
	pkgdesc = LED controller daemon for arcade cabinets and RGB lighting
	depends = tinyxml2>=6.0
	depends = libusb>=1.0.22
	depends = libpulse>=0.9
	depends = alsa-lib>=0.2

pkgname = ledspicer-nanoled
	pkgdesc = LEDSpicer plugin for Ultimarc NanoLed

pkgname = ledspicer-pacdrive
	pkgdesc = LEDSpicer plugin for Ultimarc PacDrive

pkgname = ledspicer-pacled64
	pkgdesc = LEDSpicer plugin for Ultimarc PacLed64

pkgname = ledspicer-ultimateio
	pkgdesc = LEDSpicer plugin for Ultimarc Ultimate I/O

pkgname = ledspicer-ledwiz32
	pkgdesc = LEDSpicer plugin for Groovy Game Gear LedWiz32

pkgname = ledspicer-howler
	pkgdesc = LEDSpicer plugin for WolfWareTech Howler

pkgname = ledspicer-adalight
	pkgdesc = LEDSpicer plugin for Adalight serial LEDs

pkgname = ledspicer-raspberrypi
	pkgdesc = LEDSpicer plugin for Raspberry Pi GPIO

pkgname = ledspicer-dev
	pkgdesc = LEDSpicer development headers
EOF

	echo "Generated: $outfile"
}

# Update version in PKGBUILD
update_pkgbuild_version() {
	local pkgbuild="${SCRIPT_DIR}/arch/PKGBUILD"
	if [[ -f "$pkgbuild" ]]; then
		sed -i "s/^pkgver=.*/pkgver=${VERSION}/" "$pkgbuild"
		sed -i "s/^pkgrel=.*/pkgrel=${RELEASE}/" "$pkgbuild"
		echo "Updated: $pkgbuild"
	fi
}

# Update sha256sum in PKGBUILD (uses CHECKSUM fetched earlier)
update_pkgbuild_checksum() {
	local pkgbuild="${SCRIPT_DIR}/arch/PKGBUILD"

	if [[ -f "$pkgbuild" ]]; then
		sed -i "s/^sha256sums=.*/sha256sums=('${CHECKSUM}')/" "$pkgbuild"
		echo "Updated: $pkgbuild (sha256sum)"
	fi
}

# Fetch tarball checksum from GitHub
fetch_checksum() {
	local tarball_url="${URL}/archive/refs/tags/${VERSION}.tar.gz"

	echo ""
	echo "Fetching tarball checksum from GitHub..."
	echo "  URL: $tarball_url"

	CHECKSUM=$(curl -sL "$tarball_url" 2>/dev/null | sha256sum | cut -d' ' -f1)

	# Empty file hash means tarball not found
	if [[ -z "$CHECKSUM" || "$CHECKSUM" == "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855" ]]; then
		echo "  Warning: Tag ${VERSION} not found on GitHub (or network error)"
		echo "  Using sha256sums=('SKIP') - run again after tagging"
		CHECKSUM="SKIP"
	else
		echo "  Checksum: $CHECKSUM"
	fi
}

# Update version in RPM spec
update_spec_version() {
	local specfile="${SCRIPT_DIR}/rpm/ledspicer.spec"
	if [[ -f "$specfile" ]]; then
		sed -i "s/^Version:.*/Version:        ${VERSION}/" "$specfile"
		sed -i "s/^Release:.*/Release:        ${RELEASE}%{?dist}/" "$specfile"
		echo "Updated: $specfile (version)"
	fi
}

# Update changelog in RPM spec
update_spec_changelog() {
	local specfile="${SCRIPT_DIR}/rpm/ledspicer.spec"
	if [[ ! -f "$specfile" ]]; then
		return
	fi

	local changes
	changes=$(extract_changes "$VERSION" | sed 's/^  \* /- /; s/^    - /  - /')
	local date_str
	date_str=$(date "+%a %b %d %Y")
	
	local changelog_entry="* ${date_str} ${MAINTAINER} <${EMAIL}> - ${VERSION}-${RELEASE}
${changes}"

	# Replace everything after %changelog with new entry
	sed -i '/%changelog/,$d' "$specfile"
	
	cat >> "$specfile" << EOF
%changelog
${changelog_entry}
EOF

	echo "Updated: $specfile (changelog)"
}

# =============================================================================
# Main
# =============================================================================

echo "=============================================="
echo "LEDSpicer Packaging Generator"
echo "=============================================="
echo ""
echo "All values from CMakeLists.txt:"
echo "  Version:      ${VERSION}"
echo "  Data Version: ${DATA_VERSION}"
echo "  URL:          ${URL}"
echo "  Maintainer:   ${MAINTAINER}"
echo "  Email:        ${EMAIL}"

fetch_checksum

echo ""
echo "----------------------------------------------"

generate_debian
generate_srcinfo
update_pkgbuild_version
update_pkgbuild_checksum
update_spec_version
update_spec_changelog

echo "----------------------------------------------"
echo "Done!"
